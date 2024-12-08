#include "OperationCenter.h"
#include "DataCenter.h"
#include "../monsters/Monster.h"
#include "../Player.h"
/*-----I2P Revise start-----*/
#include "../Hero.h"
/*-----I2P Revise end-----*/

void OperationCenter::update()
{
	// Update monsters.
	_update_monster();
	// Update towers.
	// If any monster reaches the end, hurt the player and delete the monster.
	_update_monster_player();
	/*-----I2P Revise start-----*/
	_update_monster_hero();
	/*-----I2P Revise end-----*/
}

void OperationCenter::_update_monster()
{
	std::vector<Monster *> &monsters = DataCenter::get_instance()->monsters;
	for (Monster *monster : monsters)
		monster->update();
}





void OperationCenter::_update_monster_player()
{
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster *> &monsters = DC->monsters;
	Player *&player = DC->player;
	for (size_t i = 0; i < monsters.size(); ++i)
	{
		// Check if the monster is killed.
		if (monsters[i]->HP <= 0)
		{
			// Monster gets killed. Player receives money.
			player->coin += monsters[i]->get_money();
			monsters.erase(monsters.begin() + i);
			--i;
			// Since the current monsster is killed, we can directly proceed to next monster.
			break;
		}
		// Check if the monster reaches the end.
		if (monsters[i]->get_path().empty())
		{
			monsters.erase(monsters.begin() + i);
			player->HP--;
			--i;
		}
	}
}
/*-----I2P Revise start-----*/
void OperationCenter::_update_monster_hero()
{
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Monster *> &monsters = DC->monsters;
	for (size_t i = 0; i < monsters.size(); ++i)
	{
		if (monsters[i]->shape->overlap(*(DC->hero->shape)))
		{
			monsters[i]->HP = 0;
		}
	}
}
/*-----I2P Revise end-----*/

void OperationCenter::draw()
{
	_draw_monster();
}


void OperationCenter::_draw_monster()
{
	std::vector<Monster *> &monsters = DataCenter::get_instance()->monsters;
	for (Monster *monster : monsters)
		monster->draw();
}

