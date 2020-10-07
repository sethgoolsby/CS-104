#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "place_result.h"
#include "move.h"
#include "exceptions.h"
#include "human_player.h"
#include "tile_kind.h"
#include "formatting.h"
#include "rang.h"
#include <sstream>

using namespace std;


// This method is fully implemented.
inline string& to_upper(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

Move HumanPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
	// TODO: begin your implementation here.
	string rawCommand;
	board.print(std::cout);
	cout<<endl<<endl<<endl << "What Would you like to do?"<<endl;
	cin.getline(rawCommand);
	parse_move(rawCommand);
}

vector<TileKind> HumanPlayer::parse_tiles(string& letters) const{
    // TODO: begin implementation here.
    vector<TileKind> tiles;
    TileKind tile;
    char letter;
    for(int i = 0; i < letters.size(); i++)
    {
    	letter = letters[i];
    	try
    	{
    	tile = hand.lookup_tile(letter);
    	}
    	catch
    	{
    		cout<< "You wrote a wrong tile that is not in the pile";
    	}
    	tiles.push_back(tile);
    }
    return tiles;
}

Move HumanPlayer::parse_move(string& move_string) const {
	// TODO: begin implementation here.
	stringstream ss(move_string);
	string command;
	ss >> command;
	if(command == "PASS")
	{
		MoveKind thing = PASS;
		Move thingy(thing);
		return thing;
	}
	else if(command == "EXCHANGE")
	{
		string letters;
		ss>>letters;
		Move thing(MoveKind::PASS,parse_tiles(letters));
		return thing;
	}
	else if(command == "PLACE")
	{
		char direction;
		size_t x,y;
		string letters;
		Direction dir;
		ss >>direction >> x >> y>> letters;
		if(direction == '|')
		{
			dir = DOWN;
		}
		else if(direction == '-')
		{
			dir = ACROSS
		}
		else
		{
			cout<< "You entered an invalid command";
			return get_move(board,dictionary);
		}

		Move thing(MoveKind::PLACE, parse_tiles(letters), x.y,dir);
		return thing;
	}
	else
	{
		cout << "You entered an invalid command";
		return get_move(board, dictionary);
	}
}


// This function is fully implemented.
void HumanPlayer::print_hand(ostream& out) const {
	const size_t tile_count = tiles.count_tiles();
	const size_t empty_tile_count = this->get_hand_size() - tile_count;
	const size_t empty_tile_width = empty_tile_count * (SQUARE_OUTER_WIDTH - 1);

	for(size_t i = 0; i < HAND_TOP_MARGIN - 2; ++i) {
		out << endl;
	}

	out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_HEADING << "Your Hand: " << endl << endl;

    // Draw top line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;

    // Draw middle 3 lines
    for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
        out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD << repeat(SPACE, HAND_LEFT_MARGIN);
        for (auto it = tiles.cbegin(); it != tiles.cend(); ++it) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_PLAYER_HAND;

            // Print letter
            if (line == 1) {
                out << repeat(SPACE, 2) << FG_COLOR_LETTER << (char)toupper(it->letter) << repeat(SPACE, 2);

            // Print score in bottom right
            } else if (line == SQUARE_INNER_HEIGHT - 1) {
                out << FG_COLOR_SCORE << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << setw(2) << it->points;

            } else {
                out << repeat(SPACE, SQUARE_INNER_WIDTH);
            }
        }
        if (tiles.count_tiles() > 0) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
            out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << rang::style::reset << endl;
}
