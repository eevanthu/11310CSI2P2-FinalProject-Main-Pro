#include "OperationCenter.h"
#include "DataCenter.h"
#include "../monsters/Monster.h"
#include "../Player.h"
//#include "Utils.h"
/*-----I2P Revise start-----*/
#include "../Hero.h"
#include "../Tank.h"
#include "../Bullet.h"
#include "../Obstacle.h"
/*-----I2P Revise end-----*/

void OperationCenter::update()
{
	// Update monsters.
	// _update_monster();
	// Update towers.
	// If any monster reaches the end, hurt the player and delete the monster.
	// _update_monster_player();
	/*-----I2P Revise start-----*/
	// _update_monster_hero();
	_update_tank_bullet();
	_update_tank_obstacle();
	_update_bullet_obstacle();
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
// void OperationCenter::_update_monster_hero()
// {
// 	DataCenter *DC = DataCenter::get_instance();
// 	std::vector<Monster *> &monsters = DC->monsters;
// 	for (size_t i = 0; i < monsters.size(); ++i)
// 	{
// 		if (monsters[i]->shape->overlap(*(DC->hero->shape)))
// 		{
// 			monsters[i]->HP = 0;
// 		}
// 	}
// }

void OperationCenter::_update_tank_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Tank *> &tanks = DC->tanks;
    std::vector<std::unique_ptr<Bullet>> &bullets = DC->bullets;

    for (size_t i = 0; i < bullets.size(); ++i)
    {
        for (size_t j = 0; j < tanks.size(); ++j)
        {
			if (bullets[i]->get_owner_id() == tanks[j]->get_id()) {
				continue;
			}
            // 檢查子彈與坦克是否重疊
            if (bullets[i]->shape->overlap(*(tanks[j]->shape)))
            {
                // 子彈擊中坦克的行為處理
				if (tanks[j]->set_num_shield(0) == 0) {
                	tanks[j]->decrease_hp(bullets[i]->get_dmg());
				}  else {
					tanks[j]->set_num_shield(-1);
				}
                bullets[i]->set_fly_dist(0);         // 設定子彈為無效
            }
        }
    }
}

void OperationCenter::_update_tank_obstacle() {
    DataCenter *DC = DataCenter::get_instance();
    std::vector<Tank *> &tanks = DC->tanks;
    std::vector<Obstacle *> &obstacles = DC->obstacles;

    for (size_t i = 0; i < tanks.size(); ++i) {
        for (size_t j = 0; j < obstacles.size(); ++j) {
			if (obstacles[j]->get_state() == ObstacleState::DESTROYED) {
				continue;
			}
			bool is_overlap = tanks[i]->shape->overlap(*(obstacles[j]->shape));
            if (is_overlap) {
				if (obstacles[j]->get_state() == ObstacleState::ITEM_MAXHP) {
					float random = ((rand() % 26) - 5);
					random /= 100;
					tanks[i]->set_max_hp(random);
					obstacles[j]->set_state();
					continue;
				} else if (obstacles[j]->get_state() == ObstacleState::ITEM_TANKSPEED) {
					tanks[i]->set_speed();
					obstacles[j]->set_state();
					continue;
				} else if (obstacles[j]->get_state() == ObstacleState::ITEM_NUMBULLET) {
					tanks[i]->set_triple_timer();
					tanks[i]->set_state(TankState::TRIPLEBULLET);
					obstacles[j]->set_state();
					continue;
				} else if (obstacles[j]->get_state() == ObstacleState::ITEM_SHIELD) {
					tanks[i]->set_num_shield(1);
					obstacles[j]->set_state();
					continue;
				} else if (obstacles[j]->get_state() == ObstacleState::ITEM_PENETRATE) {
					tanks[i]->set_num_penerate(3);
					obstacles[j]->set_state();
					continue;
				}
				tanks[i]->set_obstacle_overlap(true);
                // 簡單地反向退回一段距離
                const float reverse_distance = 1.5; // 退回的距離
                float local_dx = cos(tanks[i]->get_rotation_angle()) * reverse_distance;
                float local_dy = sin(tanks[i]->get_rotation_angle()) * reverse_distance;
				tanks[i]->set_position(Point{tanks[i]->shape->center_x() + local_dx, tanks[i]->shape->center_y() + local_dy});
            }
        }
    }
}

void OperationCenter::_update_bullet_obstacle() {
    DataCenter *DC = DataCenter::get_instance();
    std::vector<std::unique_ptr<Bullet>> &bullets = DC->bullets;
    std::vector<Obstacle *> &obstacles = DC->obstacles;

    for (size_t i = 0; i < bullets.size(); ++i) {
        for (size_t j = 0; j < obstacles.size(); ++j) {
            if (bullets[i]->shape->overlap(*(obstacles[j]->shape))) {
				switch (obstacles[j]->get_state()) {
					case ObstacleState::ITEM_MAXHP:
					case ObstacleState::ITEM_TANKSPEED:
					case ObstacleState::ITEM_NUMBULLET:
					case ObstacleState::ITEM_SHIELD:
					case ObstacleState::ITEM_PENETRATE:
					case ObstacleState::DESTROYED:
						continue;
					default: 
						if (bullets[i]->is_penetrate != 1) bullets[i]->set_fly_dist(0);
						obstacles[j]->set_state();
						break;
				}
            }
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

