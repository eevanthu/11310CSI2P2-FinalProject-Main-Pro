#include "DataCenter.h"
#include <cstring>
#include "../Player.h"
#include "../monsters/Monster.h"
/*-----I2P Revise start-----*/
#include "../Hero.h"
#include "../Tank.h"
/*-----I2P Revise end-----*/

// fixed settings
namespace DataSetting
{
	constexpr double FPS = 60;
	constexpr int window_width = 1600;
	constexpr int window_height = 900;
	constexpr int game_field_length = 1600;
}

DataCenter::DataCenter()
{
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));
	player = new Player();
	/*-----I2P Revise start-----*/
	hero = new Hero();
	/*-----I2P Revise end-----*/
}

DataCenter::~DataCenter()
{
	delete player;
	for (Monster *&m : monsters) delete m;
	for (Tank *&t : tanks) delete t;
	for (Obstacle *&o : obstacles) delete o;
}