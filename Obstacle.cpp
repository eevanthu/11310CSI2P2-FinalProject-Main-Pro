#include "Obstacle.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include "shapes/Point.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
namespace ObstacleSetting
{
    static constexpr char png_root_path[50] = "./assets/image/obstacle";
    static constexpr char png_postfix[][10] = {
        "exist",
        "orange",
        "blue",
        "pink",
        "gray",
        "maxhp",
        "tankspeed",
        "numbullet",
        "shield",
        "penetrate",
        "destroyed",
        "gem"
    };
}

Obstacle::Obstacle(const Point &p, ObstacleState state) : state(state), position(p) {}

void Obstacle::init()
{
    for (size_t type = 0; type < static_cast<size_t>(ObstacleState::OBSTACLESTATE_MAX); ++type)
    {
        char buffer[50];
        snprintf(
            buffer, sizeof(buffer), "%s/obstacle_%s.png",
            ObstacleSetting::png_root_path,
            ObstacleSetting::png_postfix[static_cast<int>(type)]);
        pngPath[static_cast<ObstacleState>(type)] = std::string{buffer};
    }
    // DataCenter *DC = DataCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();

    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    width = al_get_bitmap_width(bitmap);
    height = al_get_bitmap_height(bitmap);
    shape.reset(new Rectangle{position.x, position.y, position.x + width, position.y + width});

}

void Obstacle::set_state()
{
    int random = rand() % 100;
    switch(state) {
        case ObstacleState::EXIST:
            if (random < 10) state = ObstacleState::STONE;
            else if (random < 70) state = ObstacleState::ORANGE;
            else if (random < 100) state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::ORANGE:
            if (random < 10) state = ObstacleState::STONE;
            else if (random < 20) state = ObstacleState::ITEM_MAXHP;
            else if (random < 60) state = ObstacleState::PINK;
            else if (random < 100) state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::PINK:
            if (random < 10) state = ObstacleState::STONE;
            else if (random < 15) state = ObstacleState::ITEM_TANKSPEED;
            else if (random < 30) state = ObstacleState::ITEM_NUMBULLET;
            else if (random < 40) state = ObstacleState::ITEM_SHIELD;
            else if (random < 80) state = ObstacleState::BLUE;
            else if (random < 100) state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::BLUE:
            if (random < 1) state = ObstacleState::STONE;
            else if (random < 50) state = ObstacleState::ITEM_PENETRATE;
            else if (random < 99 && mode == 0) state = ObstacleState::GEM;
            else if (random < 100) state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::DESTROYED:
            // state = ObstacleState::EXIST;
            break;
        case ObstacleState::ITEM_MAXHP:
            state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::ITEM_TANKSPEED:
            state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::ITEM_NUMBULLET:
            state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::ITEM_SHIELD:
            state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::ITEM_PENETRATE:
            state = ObstacleState::DESTROYED;
            break;
        case ObstacleState::STONE:
            break;
        case ObstacleState::GEM:
            state = ObstacleState::DESTROYED;
            break;
        default: break;
    }
}

void Obstacle::draw()
{
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    if (!bitmap) {
        std::cerr << "Failed to load PNG: " << pngPath[state] << std::endl;
        return;
    }
	al_draw_bitmap(
		bitmap,
		shape->center_x() - al_get_bitmap_width(bitmap)/2,
		shape->center_y() - al_get_bitmap_height(bitmap)/2, 0);
}