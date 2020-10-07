#include "board.h"
#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <iomanip>
#include <fstream>

using namespace std;


bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const {
    return this->translate(direction, 1);
}

Board::Position Board::Position::translate(Direction direction, ssize_t distance) const {
    if (direction == Direction::DOWN) {
        return Board::Position(this->row + distance, this->column);
    } else {
        return Board::Position(this->row, this->column + distance);
    }
}

Board Board::read(const string& file_path) {
    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open board file!");
    }

    size_t rows;
    size_t columns;
    size_t starting_row;
    size_t starting_column;
    file >> rows >> columns >> starting_row >> starting_column;
    Board board(rows, columns, starting_row, starting_column);

    // TODO: complete implementation of reading in board from file here.
    char tileType;
    for(int row = 0; row < rows; row++)
    {
    	for(int col = 0; col < columns; col++)
    	{
    		file >> tileType;
    		if(tileType == '.')
    		{
    		this->squares[row][col].letter_multiplier = 1;
    		this->squares[row][col].word_multiplier = 1;
    		}
    		else if(tileType == '2')
    		{
    		squares[row][col].letter_multiplier = 2;
    		squares[row][col].word_multiplier = 1;	
    		}
    		else if(tileType == '3')
    		{
    		squares[row][col].letter_multiplier = 3;
    		squares[row][col].word_multiplier = 1;	
    		}
    		else if(tileType == 'd')
    		{
    		squares[row][col].letter_multiplier = 1;
    		squares[row][col].word_multiplier = 2;	
    		}
    		else if(tileType == 't')
    		{
    		squares[row][col].letter_multiplier = 1;
    		squares[row][col].word_multiplier = 3;	
    		}
    	}
    }

    return board;
}

size_t Board::get_move_index() const {
    return this->move_index;
}

PlaceResult Board::test_place(const Move& move) const {
    // TODO: complete implementation here
    /*
	// This will check placement only it will not check if the letters are in a person's hand, or other cases.
    */
    PlaceResult result;
    if(move.MoveKind == PLACE) // We only will handle PLACE rules here
    {
    	//first : find all possible words created by placement
    	//second : check for floating or overlapping case
    	//third: check for out of bounds.
    	bool floating_flag = false; //flag will be false if word isn't connected in and turns false if it connects in
    	if(move.Direction == ACROSS)
    	{
    		size_t row = move.row;
    		for(size_t col = move.column; col < (move.column+move.tiles.size()); col++)
    		{
    			Position pos(row, col);
    			Position up(row -1, col);
    			Position down(row+1, col);

    			if(this->in_bounds_and_has_tile(pos)) //checks to see if in bounds and has a tile
    			{
    				if(move.tiles[col - move.column] != this->at(pos).get_tile_kind().letter)//if the tile is a mismatch for the one already placed...
    				{
    					result.valid = false;
    					result.error = "Mismatching Letter Placement";
    					return result
    				}
    				else
    				{
    					floating_flag = true;
    				}
    			}
    			else if(!this->is_in_bounds(pos)) // if not in bounds
    			{
    				result.valid = false;
    				result.error = "Word goes out of bounds";
    				return result;
    			}
    			else
    			{
    				if(this->in_bounds_and_has_tile(up)) // There is a possible word going from left to right
    				{
    					floating_flag = true;
    					std::string word = "";
    					Position newWordPos(row,col);
    					while(this->in_bounds_and_has_tile(newWordPos)) // finds the start of the word
    					{
    						newWordPos.row = newWordPos.row-1;
    					}
    					if(!this->in_bounds_and_has_tile(newWordPos))
    					{
    					newWordPos.row = newWordPos.row+1;
    					}
    					while(this->in_bounds_and_has_tile(newWordPos)) // finds the start of the word
    					{
    						word = word.push_back(this->at(newWordPos).get_tile_kind().letter);
    						newWordPos.row = newWordPos.row+1;
    					}
    					result.words.push_back(word);
    				}
    				else if(this->in_bounds_and_has_tile(down))
    				{
    					floating_flag = true;
    					std::string word = "";
    					Position newWordPos(row,col);
    					while(this->in_bounds_and_has_tile(newWordPos))
    					{
    						word = word.push_back(this->at(newWordPos).get_tile_kind().letter);
    						newWordPos.row = newWordPos.row+1;
    					}
    				}		
				}
    		}
    		//check edge case at begining and end of the word as well as add the word to the list of words
    		Position edge(row, move.column -1);
    		Position edge2(row, move.column + move.tiles.size());
    		if(this->in_bounds_and_has_tile(edge))
    		{
    			floating_flag = true;   
    			while(this->in_bounds_and_has_tile(edge))
    			{
    				edge.column = edge.column - 1;
    			} 			
    		}
    		else
    		{
    			edge.column = move.column;
    		}
    		if(this->in_bounds_and_has_tile(edge2))
    		{
    			floating_flag = true;
    		}
    		
    		std::string word = "";
    		while(this->in_bounds_and_has_tile(edge))
    		{
    			word = word.push_back(this->at(edge).get_tile_kind().letter)
    			edge.column = edge.column + 1;
    		}
    		result.words.push_back(word);
    		//end edge case check
    		if(floating_flag == false)
    		{
    			result.valid = false;
    			result.error = "The word was floating and didn't connect with anything";
    		}
    		else
    		{
    			result.valid = true;
    		}
    		return result;
    	}
    	else if(move.Direction == DOWN)
    	{
			size_t col = move.column;

    		for(size_t row = move.row; row < (move.row+move.tiles.size()); row++)
    		{
    			Position pos(row,col);
    			Position left(row, col -1);
    			Position right(row, col + 1);

    			if(this->in_bounds_and_has_tile(pos)) //checks to see if in bounds and has a tile
    			{
    				if(move.tiles[row - move.row] != this->at(pos).get_tile_kind().letter)//if the tile is a mismatch for the one already placed...
    				{
    					result.valid = false;
    					result.error = "Mismatching Letter Placement";
    					return result
    				}
    				else
    				{
    					floating_flag = true;
    				}
    			}
    			else if(!this->is_in_bounds(pos)) // if not in bounds
    			{
    				result.valid = false;
    				result.error = "Word goes out of bounds";
    				return result;
    			}
    			else //this is in bounds but is empty
    			{
    				//check for surrounding tiles know that in this part, we have already determined that the player is adding a letter to the word: this word counts for points, if there was already a tile, it doesn't count for points
    				if(this->in_bounds_and_has_tile(left)) // There is a possible word going from left to right
    				{
    					floating_flag = true;
    					std::string word = "";
    					Position newWordPos(row,col);
    					while(this->in_bounds_and_has_tile(newWordPos)) // finds the start of the word
    					{
    						newWordPos.column = newWordPos.column-1;
    					}
    					if(!this->in_bounds_and_has_tile(newWordPos))
    					{
    					newWordPos.column = newWordPos.column+1;
    					}
    					while(this->in_bounds_and_has_tile(newWordPos)) // finds the start of the word
    					{
    						word = word.push_back(this->at(newWordPos).get_tile_kind().letter);
    						newWordPos.column = newWordPos.column+1;
    					}
    					result.words.push_back(word);
    				}
    				else if(this->in_bounds_and_has_tile(right))
    				{
    					floating_flag = true;
    					std::string word = "";
    					Position newWordPos(row,col);
    					while(this->in_bounds_and_has_tile(newWordPos))
    					{
    						word = word.push_back(this->at(newWordPos).get_tile_kind().letter);
    						newWordPos.column = newWordPos.column+1;
    					}
    				}
    			}
    		}

    		//Edge cases 
    		Position edge(move.row - 1, col);
    		Position edge2(move.row + move.tiles.size(),col);
    		if(this->in_bounds_and_has_tile(edge))
    		{
    			floating_flag = true;
    			while(this->in_bounds_and_has_tile(edge))
    			{
    				edge.row = edge.row -1;
    			}
    		}
    		else
    		{
    			edge.row = move.row;
    		}
    		if(this->in_bounds_and_has_tile(edge2))
    		{
    			floating_flag = true;
    		}

    		std::string word = "";
    		while(this->in_bounds_and_has_tile(edge))
    		{
    			word = word.push_back(this->at(edge).get_tile_kind().letter)
    			edge.row = edge.row + 1;
    		}
    		result.words.push_back(word);

    		if(floating_flag == false)
    		{
    			result.valid = false;
    			result.error = "The word was floating and didn't connect with anything";
    		}
    		else
    		{
    			result.valid = true;
    		}
    		return result;
    	}
    	else
    	{
    		result.valid = false;
    		result.error = "The direcction was not accross or down..";
    		return result;
    	}
    }
}

PlaceResult Board::place(const Move& move)
{
    // TODO: Complete implementation here
 PlaceResult result;
 unsigned int points = 0;
 unsigned int extraPoints = 0;
 unsigned int wordMultiplier = 1;
 if(move.direction == Direction::ACROSS)
 {
 	Position right(move.row, move.column -1);
 	if(in_bounds_and_has_tile(right)) // extra check if it is adding on to a word from before
 	{
 		while(in_bounds_and_has_tile(right))
 		{
 			extraPoints = extraPoints + squares[right.row][right.column].get_tile_kind().points;
 			right.column = right.column -1;
 		}
 	}
 	for(size_t i = move.column; (i < move.column + move.tiles.size()); i++)
 	{
 		Position top(move.row-1, i);
 		Position bottom(move.row+1, i);
 		Position tile(move.row,i);
 		 //a check for other words that are not the main boi
 		if((in_bounds_and_has_tile(top) || in_bounds_and_has_tile(bottom)) && (!in_bounds_and_has_tile(tile)))
 		{//this means there is a "word" that is added to from the top or from the bottom.
 		//according to the official rule book, letters that make up 2 sperate words are counted twice.
 			while(in_bounds_and_has_tile(top))
 			{
 				top.row = top.row -1;
 			}
 			if(!in_bounds_and_has_tile(top))
 			{
 				top.row = top.row + 1;
 			}
 			while(in_bounds_and_has_tile(top))
 			{
 				extraPoints = extraPoints + squares[top.row][top.column].get_tile_kind().points;
 				top.row = top.row + 1;
 			}
 		}
 		squares[move.row][i].set_tile_kind(move.tiles[i-move.column]);
 		points = points + (move.tiles[i-move.column].points*squares[move.row][i].letter_multiplier);
 		wordMultiplier = wordMultiplier*squares[move.row][i].word_multiplier;
 	}
 	Position left(move.row, move.column + move.tiles.size());
 	if(in_bounds_and_has_tile(left))
 	{
 		while(in_bounds_and_has_tile(left))
 		{
 			extraPoints = extraPoints + squares[right.row][right.column].get_tile_kind().points;
 			right.column = right.column + 1;
 		}
 	}
 	points = points * wordMultiplier;

 }
 else if(move.direction == Direction::DOWN)
 {
 	Position top(move.row -1, move.column);
 	if(in_bounds_and_has_tile(top))
 	{
 		while(in_bounds_and_has_tile(top))
 		{
 			extraPoints = extraPoints + squares[top.row][top.column].get_tile_kind().points;
 			top.row = top.row -1;
 		}
 	}
 	for(size_t i = move.row; i < (move.row + move.tiles.size()); i++)
 	{
 		Position left(i, move.column -1);
 		Position right(i,move.column +1);
 		Position tile(i, move.column);
 		 //a check for other words that are not the main boi
 		if((in_bounds_and_has_tile(left) || in_bounds_and_has_tile(right)) && (!in_bounds_and_has_tile(tile)))
 		{//this means there is a "word" that is added to from the top or from the bottom.
 		//according to the official rule book, letters that make up 2 sperate words are counted twice.
 			while(in_bounds_and_has_tile(left))
 			{
 				left.column = left.column -1;
 			}
 			if(!in_bounds_and_has_tile(left))
 			{
 				left.column = left.column + 1;
 			}
 			while(in_bounds_and_has_tile(left))
 			{
 				extraPoints = extraPoints + squares[left.row][left.column].get_tile_kind().points;
 				left.column = top.row + 1;
 			}
 		}
 		squares[i][move.column].set_tile_kind(move.tiles[i-move.row]);
 		points = points + (move.tiles[i-move.row].points*squares[i][move.column].letter_multiplier);
 		wordMultiplier = wordMultiplier*squares[i][move.column].word_multiplier;
 	}
 	Position bottom(move.row + move.tiles.size(),move.column);
 	if(in_bounds_and_has_tile(bottom))
 	{
 		while(in_bounds_and_has_tile(bottom))
 		{
 			extraPoints = extraPoints + squares[bottom.row][bottom.column].get_tile_kind().points;
 			bottom.row = bottom.row + 1;
 		}
 	}
 	points = points * wordMultiplier;
 }
 result.points = points + extraPoints;
 return result;
}

// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) {
    return this->squares.at(position.row).at(position.column);
}

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const{
    return is_in_bounds(position) && at(position).has_tile();
}

void Board::print(ostream& out) const {
	// Draw horizontal number labels
	for (size_t i = 0; i < BOARD_TOP_MARGIN - 2; ++i) {
		out << std::endl;
	}
	out << FG_COLOR_LABEL << repeat(SPACE, BOARD_LEFT_MARGIN);
	const size_t right_number_space = (SQUARE_OUTER_WIDTH - 3) / 2;
	const size_t left_number_space = (SQUARE_OUTER_WIDTH - 3) - right_number_space;
	for (size_t column = 0; column < this->columns; ++column) {
		out << repeat(SPACE, left_number_space) << std::setw(2) << column + 1 << repeat(SPACE, right_number_space);
	}
	out << std::endl;

	// Draw top line
	out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << endl;

	// Draw inner board
	for (size_t row = 0; row < this->rows; ++row) {
        if (row > 0) {
            out << repeat(SPACE, BOARD_LEFT_MARGIN);
			print_horizontal(this->columns, T_RIGHT, PLUS, T_LEFT, out);
			out << endl;
		}

		// Draw insides of squares
		for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
			out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD;

			// Output column number of left padding
			if (line == 1) {
				out << repeat(SPACE, BOARD_LEFT_MARGIN - 3);
				out << std::setw(2) << row + 1;
				out << SPACE;
			} else {
				out << repeat(SPACE, BOARD_LEFT_MARGIN);
			}

            // Iterate columns
			for (size_t column = 0; column < this->columns; ++column) {
				out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
				const BoardSquare& square = this->squares.at(row).at(column);
				bool is_start = this->start.row == row && this->start.column == column;

				// Figure out background color
				if (square.word_multiplier == 2) {
					out << BG_COLOR_WORD_MULTIPLIER_2X;
				} else if (square.word_multiplier == 3) {
					out << BG_COLOR_WORD_MULTIPLIER_3X;
				} else if (square.letter_multiplier == 2) {
					out << BG_COLOR_LETTER_MULTIPLIER_2X;
				} else if (square.letter_multiplier == 3) {
					out << BG_COLOR_LETTER_MULTIPLIER_3X;
				} else if (is_start) {
					out << BG_COLOR_START_SQUARE;
                }

				// Text
                if (line == 0 && is_start) {
                    out << "  \u2605  ";
                } else if (line == 0 && square.word_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1) << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1) << square.letter_multiplier;
				} else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l << repeat(SPACE, 1);
				} else if (line == SQUARE_INNER_HEIGHT - 1 && square.has_tile()) {
					out << repeat(SPACE, SQUARE_INNER_WIDTH - 1) << FG_COLOR_SCORE << square.get_points();
				} else {
					out << repeat(SPACE, SQUARE_INNER_WIDTH);
				}
			}

			// Add vertical line
			out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_OUTSIDE_BOARD << std::endl;
		}
	}

	// Draw bottom line
	out << repeat(SPACE, BOARD_LEFT_MARGIN);
	print_horizontal(this->columns, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
	out << endl << rang::style::reset << std::endl;
}
