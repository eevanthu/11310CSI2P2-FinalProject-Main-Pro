#include "Bullet.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Circle.h"
#include "shapes/Point.h"
#include <algorithm>
#include <allegro5/bitmap_draw.h>
#include <iostream>
#include <cmath>

Bullet::Bullet(double x, double y, const float& angle, double speed) {
    ImageCenter *IC = ImageCenter::get_instance();
    this->fly_dist = 100000;
    this->dmg = 10;
    rotation_angle = angle;
    bitmap = IC->get("./assets/image/bullet.png");
    vx = speed * cos(angle);
    vy = speed * sin(angle);

    shape.reset(new Circle{x, y, double(5)});

    if (!bitmap) {
        std::cerr << "failed to load bullet image." << std::endl;
        return;
    }
}

void Bullet::update() {
    if (fly_dist <= 0) return;
    DataCenter *DC = DataCenter::get_instance();
    double dx = vx / DC->FPS;
    double dy = vy / DC->FPS;

    shape->update_center_x(shape->center_x() - dx);
    shape->update_center_y(shape->center_y() - dy);

    fly_dist -= sqrt(vx * vx + vy * vy);
}

void Bullet::draw() {
    if (!bitmap) return;
    /*
    al_draw_bitmap(
        bitmap,
        shape->center_x() - al_get_bitmap_width(bitmap) / 2,
        shape->center_y() - al_get_bitmap_height(bitmap) / 2, 0);
    */
    al_draw_rotated_bitmap(
        bitmap,
        al_get_bitmap_width(bitmap) / 2,
        al_get_bitmap_height(bitmap) / 2,
        shape->center_x(),
        shape->center_y(),
        rotation_angle,
    0);
}

bool Bullet::is_out_of_range() const {
    return fly_dist <= 0;
}