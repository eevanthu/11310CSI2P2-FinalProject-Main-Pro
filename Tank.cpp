#include "Tank.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include <iostream>
#include <stdio.h>
#include <cmath>

namespace TankSetting
{
    static constexpr char png_root_path[50] = "./assets/image/tank";
    static constexpr char png_postfix[][10] = {
        "alive",
        "dead"
    };

}

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


    // 設定初始角度
    rotation_angle = M_PI / 2;
    rotation_left = 1;
    angular_speed = 0.08f;
    speed = 3;
    moving_forward = false;
    TankState state = TankState::ALIVE;

    // 設定圖片大小
    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    width = al_get_bitmap_width(bitmap);
    height = al_get_bitmap_height(bitmap);
    shape.reset(new Rectangle{DC->window_width / 2,
                              DC->window_height / 2,
                              DC->window_width / 2 + width,
                              DC->window_height / 2 + height});
}

void Tank::update() {
    DataCenter *DC = DataCenter::get_instance();
    static bool last_key_state_w = false;

    if (DC->key_state[ALLEGRO_KEY_W]) {
        // 按下 W 鍵，坦克向前移動
        moving_forward = true;
        if (last_key_state_w == false) {
            rotation_left *= -1;
        }
    } else {
        // 沒有按下 W 鍵，坦克停止移動
        moving_forward = false;
    }

    last_key_state_w = DC->key_state[ALLEGRO_KEY_W];

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
}
