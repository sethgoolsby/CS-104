#include "player.h"

using namespace std;


// TODO: implement member functions

void player::add_points(size_t points)
{
	this->points = this->points + points;
}
void player::subtract_points(size_t points)
{
	this->points = this->points - points;
}
size_t player::get_points() const
{
	return this->points;
}
const std::string& player::get_name() const
{
	return this->name;
}
 size_t player::count_tiles() const
 {
 	return hand_size;
 }
void player::remove_tiles(const std::vector<TileKind>& tiles)
  {
  	std::vector<TileKind>::iterator it = tiles.begin()
  	for(it; it =! tiles.end(); ++it)
  	{
  	this->hand.remove_tile(*it);
  	}
  }
  void player::add_tiles(const std::vector<TileKind>& tiles)
  {
  	std::vector<TileKind>::iterator it = tiles.begin()
  	for(it; it =! tiles.end(); ++it)
  	{
  	this->hand.add_tile(*it);
  	}
  }
bool player::has_tile(TileKind tile)
{
	try 
	{
		hand.lookup_tile(tile);
	}
	catch(std::exception& e)
	{
		return false;
	}
	return true;
}
unsigned int player::get_hand_value() const
{
	return hand.total_points();	
}
size_t player::get_hand_size() const
{
	return hand.count_tiles();
}