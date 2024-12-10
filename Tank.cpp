#include "Tank.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include "./shapes/Point.h"
#include "Bullet.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>

namespace TankSetting {
    static constexpr char png_root_path[50] = "./assets/image/tank";
    static constexpr char png_postfix[][10] = {"alive", "alive", "dead"};
}

Tank::Tank(const Point &p, const ControlScheme &controlScheme) 
 : position(p), controlScheme(controlScheme) , state(TankState::ALIVE),
      rotation_angle(0.0f), rotation_left(1), angular_speed(0.1), speed(3.5),
      moving_forward(false) {}

void Tank::init() {
    for (size_t type = 0; type < static_cast<size_t>(TankState::TANKSTATE_MAX); ++type)
    {
        char buffer[50];
        sprintf(
            buffer, "%s/tank_%s.png",
            TankSetting::png_root_path,
            TankSetting::png_postfix[static_cast<int>(type)]);
        pngPath[static_cast<TankState>(type)] = std::string{buffer};
    }
    DataCenter *DC = DataCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();

    // 設定圖片大小
    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    width = al_get_bitmap_width(bitmap);
    height = al_get_bitmap_height(bitmap);
    shape.reset(new Rectangle{position.x, position.y, position.x + width, position.y + height});
}

void Tank::fire_bullet() {
    DataCenter *DC = DataCenter::get_instance();

    const double bullet_speed = 1000;

    float bullet_x = shape->center_x() - (width / 2) * cos(rotation_angle);
    float bullet_y = shape->center_y() - (width / 2) * sin(rotation_angle);

    DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle, bullet_speed));
    // bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle + 0.08, bullet_speed));
    // bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle + 0.16, bullet_speed));
}

void Tank::stun() {
    state = TankState::STUNNED;
    stun_timer = 5;
}


void Tank::update() {
    DataCenter *DC = DataCenter::get_instance();

    switch (state)
    {
        case TankState::ALIVE: {
            stun_timer = 0;
            angular_speed = 0.08f;
            speed = 3.5;
            break;
        };
        case TankState::DEAD: break;
        case TankState::STUNNED: {
            stun_timer -= (1 / DC->FPS);
            angular_speed = 1.0f;
            speed = 2.5;
            if (stun_timer <= 0) { state = TankState::ALIVE;}
            break;
        }
    }

    if (DC->key_state[controlScheme.rotate]) {
        // 按下 W 鍵，坦克向前移動，且發射子彈
        moving_forward = true;
        if (!DC->prev_key_state[controlScheme.rotate]) {
            rotation_left *= -1;
            fire_bullet();
        }
    } else if (DC->key_state[controlScheme.stun] && !DC->prev_key_state[controlScheme.stun]) {
        stun();
    } else {
        // 沒有按下 W 鍵，坦克停止移動
        moving_forward = false;
    }

    if (moving_forward == true) {
        float radian = rotation_angle;
        float dx = speed * cos(radian);
        float dy = speed * sin(radian);

        shape->update_center_x(shape->center_x() - dx);
        shape->update_center_y(shape->center_y() - dy);

    } else {
        rotation_angle += (rotation_left * angular_speed);
        if (rotation_angle >= 2 * M_PI) {
            rotation_angle -= 2 * M_PI;
        } else if (rotation_angle < 0.0f) {
            rotation_angle += 2 * M_PI;
        }
    }

    // for (auto it = bullets.begin(); it != bullets.end(); ) {
    //     (*it)->update();
    //     if ((*it)->get_fly_dist() <= 0) it = bullets.erase(it);
    //     else ++it;
    // }   
}

void Tank::draw() {
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    if (!bitmap) {
        std::cerr << "Failed to load PNG: " << pngPath[state] << std::endl;
        return;
    }
    al_draw_rotated_bitmap(
        bitmap,
        al_get_bitmap_width(bitmap) / 2,
        al_get_bitmap_height(bitmap) / 2,
        shape->center_x(),
        shape->center_y(),
        rotation_angle,
        0);
    // for (auto &bullet : bullets) {
    //     bullet->draw();
    // }
}
