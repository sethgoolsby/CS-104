#include "scrabble.h"
#include "formatting.h"
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;


// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
    : hand_size(config.hand_size)
    , minimum_word_length(config.minimum_word_length)
    , tile_bag(TileBag::read(config.tile_bag_file_path, config.seed))
    , board(Board::read(config.board_file_path))
    , dictionary(Dictionary::read(config.dictionary_file_path)) {}


// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() 
{
    // TODO: implement this.
    bool dontQuitTheLoop = true;
    unsigned short passCounter = 0;
    bool firstTimeFlag = true;
    while(dontQuitTheLoop)
    {
        for(size_t p = 0; p < players.size(); p++)
        {
            PlaceResult result("error message");
            result.valid = false;
            Move moveToExecute;
            while(result.valid = false) // command error checking complete including placement for the first time
            {
                Move playerMove = (*players[p]).get_move(board, dictionary);
                if(playerMove.kind  == MoveKind::PLACE)
                {
                    result = board.test_place(playerMove);
                    for(size_t t = 0; t < playerMove.tiles.size();t++)
                    {
                        if((*players[p]).has_tile(playerMove.tiles[t]) == false)
                        {
                            result.valid = false;
                            result.error = "You don't have the right tiles to place that.";
                        }
                    }
                   
                    for(size_t i = 0; i < result.words.size(); i++)
                    {
                        if(!dictionary.is_word(result.words[i]))
                        {
                            result.valid = false;
                            result.error = "Spelling mistake";
                        }
                    }
                    if(firstTimeFlag == true)
                    {
                        if(result.error ==  "The word was floating and didn't connect with anything")
                        {
                            if(playerMove.row == board.start.row && playerMove.column == board.start.column)
                            {
                                result.valid = true;
                                firstTimeFlag = false;
                            }
                            else
                            {
                                result.valid = false;
                                result.error = "You need to start on the Start Square!";
                            }
                        }
                    }
                }
                else if(playerMove.kind == MoveKind::EXCHANGE)
                {
                    bool flag = true;
                    for(size_t t = 0; t < playerMove.tiles.size();t++)
                    {
                        if((*players[p]).has_tile(playerMove.tiles[t]) == false)
                        {
                            result.valid = false;
                            result.error = "You are exchanging letters you don't have.";
                            flag = false;
                        }
                    }
                    if((tile_bag.count_tiles() > playerMove.tiles.size())&&flag)
                        {
                        result.valid = true;
                        }
                        else
                        {
                            result.valid = false;
                            result.error = "There aren't enough tiles in the bag.";
                        }
                }
                else if(playerMove.kind == MoveKind::PASS)
                {
                    result.valid = true;
                }
                if(result.valid == false)
                {
                    cout<<"You've made an invalid command: "<<result.error << endl;
                }
                else
                {
                    moveToExecute = playerMove;
                }
            }
            if(moveToExecute.kind == MoveKind::PLACE)
            {
              result = board.place(moveToExecute);
              (*players[p]).remove_tiles(moveToExecute.tiles);
              if(tile_bag.count_tiles() > moveToExecute.tiles.size())
              {
                (*players[p]).add_tiles(tile_bag.remove_random_tiles(moveToExecute.tiles.size()));
              }
                else
                {
                    (*players[p]).add_tiles(tile_bag.remove_random_tiles(tile_bag.count_tiles()));
                    if((*players[p]).count_tiles() == 0)
                    {
                        dontQuitTheLoop = false;
                    }
                }


              passCounter = 0;
              (*players[p]).add_points(result.points);
              if(moveToExecute.tiles.size() >= (*players[p]).get_hand_size())
              {
                (*players[p]).add_points(EMPTY_HAND_BONUS);
              }
               cout << "You gained " << SCORE_COLOR << result.points << rang::style::reset << " points!" << endl;
               cout << "Your current score: " << SCORE_COLOR << (*players[p]).get_points() << rang::style::reset << endl;
               cout << endl << "Press [enter] to continue.";
               while(cin.ignore(256,'\n'))
               {}
            }
           else if(moveToExecute.kind == MoveKind::EXCHANGE)
            {
               (*players[p]).remove_tiles(moveToExecute.tiles);
               (*players[p]).add_tiles(tile_bag.remove_random_tiles(moveToExecute.tiles.size()));
               passCounter = 0;
               cout << "Your current score: " << SCORE_COLOR << (*players[p]).get_points() << rang::style::reset << endl;
               cout << endl << "Press [enter] to continue.";
               while(cin.ignore(256,'\n'))
               {}
            }
            else
            {
                passCounter++;
                if(passCounter > players.size())
                {
                    dontQuitTheLoop = false;
                }
               cout << "Your current score: " << SCORE_COLOR << (*players[p]).get_points() << rang::style::reset << endl;
               cout << endl << "Press [enter] to continue.";
               while(cin.ignore(256,'\n'))
               {}
            }
        }      
    }   // Useful cout expressions with fancy colors. Expressions in curly braces, indicate values you supply.
    // cout << "You gained " << SCORE_COLOR << {points} << rang::style::reset << " points!" << endl;
    // cout << "Your current score: " << SCORE_COLOR << {points} << rang::style::reset << endl;
    // cout << endl << "Press [enter] to continue.";
}
// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>> & plrs) {

    // TODO: implement this method.
    // Do not change the method signature.
    std::vector<shared_ptr<Player>> emptyHanders;
    int sum = 0;
    for(size_t i =0; i < plrs.size(); i++)
    {
        if((*plrs[i]).get_hand_size() == 0)
        {
            emptyHanders.push_back(plrs[i]);
        }
        else
        {
            sum += (*plrs[i]).get_hand_value();
            (*plrs[i]).subtract_points((*plrs[i]).get_hand_value());
        }
    }
    for(size_t i = 0; i < emptyHanders.size();i++)
    {
        (*emptyHanders[i]).add_points(sum);
    }
}

// You should not need to change this function.
void Scrabble::print_result() {
	// Determine highest score
    size_t max_points = 0;
	for (auto player : this->players) {
		if (player->get_points() > max_points) {
			max_points = player->get_points();
        }
	}

	// Determine the winner(s) indexes
	vector<shared_ptr<Player>> winners;
	for (auto player : this->players) {
		if (player->get_points() >= max_points) {
			winners.push_back(player);
        }
	}

    cout << (winners.size() == 1 ? "Winner:" : "Winners: ");
	for (auto player : winners) {
		cout << SPACE << PLAYER_NAME_COLOR << player->get_name();
	}
	cout << rang::style::reset << endl;

	// now print score table
    cout << "Scores: " << endl;
    cout << "---------------------------------" << endl;

	// Justify all integers printed to have the same amount of character as the high score, left-padding with spaces
    cout << setw(static_cast<uint32_t>(floor(log10(max_points) + 1)));

	for (auto player : this->players) {
		cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR << player->get_name() << rang::style::reset << endl;
	}
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}
