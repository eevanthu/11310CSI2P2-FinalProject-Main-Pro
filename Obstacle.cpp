#include "Obstacle.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include "shapes/Point.h"
#include <iostream>
#include <stdio.h>
namespace ObstacleSetting
{
    static constexpr char png_root_path[50] = "./assets/image/obstacle";
    static constexpr char png_postfix[][10] = {
        "exist",
        "destroyed"
    };
}

Obstacle::Obstacle(const Point &p) : state(ObstacleState::EXIST), position(p) {}

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
    shape.reset(new Rectangle{position.x, position.y, position.x + width, position.y + height});

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