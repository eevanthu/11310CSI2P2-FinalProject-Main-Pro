#define _USE_MATH_DEFINES
#include "Tank.h"
#include "Utils.h"
#include <allegro5/allegro_primitives.h>
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include "shapes/Circle.h"
#include "./shapes/Point.h"
#include "Game.h"
#include "Bullet.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>

namespace TankSetting {
    static constexpr char png_root_path[50] = "./assets/image/tank";
    static constexpr char png_postfix[][10] = {"alive", "triple","alive", "alive", "dead"};
}

Tank::Tank(const Point &p, const ControlScheme &controlScheme) 
 : position(p), controlScheme(controlScheme) , state(TankState::ALIVE),
      rotation_angle(0.0f), rotation_left(1), angular_speed(0.1), speed(3.5),
      moving_forward(false), is_obstacle_overlap(false), hp(7200), max_hp(7200), 
      num_bullets(6), num_shield(0), num_penerate(0)
      { if (controlScheme.rotate == ALLEGRO_KEY_W) id = 1; else id = 2; }

void Tank::init() {
    for (size_t type = 0; type < static_cast<size_t>(TankState::TANKSTATE_MAX); ++type)
    {
        char buffer[50];
        snprintf(
            buffer, sizeof(buffer), "%s/tank_%s.png",
            TankSetting::png_root_path,
            TankSetting::png_postfix[static_cast<int>(type)]);
        pngPath[static_cast<TankState>(type)] = std::string{buffer};
    }
    ImageCenter *IC = ImageCenter::get_instance();

    // 設定圖片大小
    ALLEGRO_BITMAP *bitmap = IC->get(pngPath[state]);
    width = al_get_bitmap_width(bitmap);
    height = al_get_bitmap_height(bitmap);
    shape.reset(new Rectangle{position.x, position.y, position.x + width / 2, position.y + height / 2});
    //shape.reset(new Circle{(int)(position.x + width / 2), (int)(position.y + width / 2), 12});
    // shape.reset(new Circle{int(position.x + width / 2), int(position.y + height / 2), width / 2});
    
}

void Tank::fire_bullet() {
    if (num_bullets <= 0) return;
    DataCenter *DC = DataCenter::get_instance();

    float bullet_x = shape->center_x() - (width / 2) * cos(rotation_angle);
    float bullet_y = shape->center_y() - (width / 2) * sin(rotation_angle);

    if (state == TankState::TRIPLEBULLET) {
        if (num_penerate > 0) {
            num_penerate--;
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle, id, 1));
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle + 0.04f, id, 1));
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle - 0.04f, id, 1));
        } else {
            num_bullets--;
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle, id, 0));
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle + 0.04f, id, 1));
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle - 0.04f, id, 1));

        }
    } else {
        if (num_penerate > 0) {
            num_penerate--;
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle, id, 1));
        } else {
            num_bullets--;
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, rotation_angle, id, 0));
        }
    }
}

void Tank::stun() {
    state = TankState::STUNNED;
    stun_timer = 3;
}


void Tank::update() {
    DataCenter *DC = DataCenter::get_instance();

    if (hp <= 0 && mode == 0) { hp = max_hp = 7200; num_gem = 0; } // score mode
    if (hp <= 0 && mode == 1) state = TankState::DEAD;  // kill mode

    if (num_bullets < 6) bullet_timer -= (1 / DC->FPS);
    if (bullet_timer <= 0) {
        num_bullets++;
        bullet_timer = 0.1; // 子彈冷卻時間
    }
    if (hp > max_hp) hp = max_hp;

    if (DC->key_state[ALLEGRO_KEY_Z] && !DC->prev_key_state[ALLEGRO_KEY_Z]) {
        float bullet_x = shape->center_x() - (width / 2) * cos(rotation_angle);
        float bullet_y = shape->center_y() - (width / 2) * sin(rotation_angle);
        for (float i = 0; i <= 2 * M_PI; i += 0.08) {
            DC->bullets.push_back(std::make_unique<Bullet>(bullet_x, bullet_y, i, id, 1));
        }
    }

    switch (state)
    {
        case TankState::TRIPLEBULLET : {
            triple_bullet_timer -= (1 / DC->FPS);
            if (triple_bullet_timer <= 0) {
                state = TankState::ALIVE;
            }
        }
        case TankState::ALIVE : {
            stun_timer = 0;
            angular_speed = 0.08f;
            if (DC->key_state[controlScheme.rotate]) {
                // 按下 W 鍵，坦克向前移動，且發射子彈
                hp_timer = 3;
                moving_forward = true;
                if (!DC->prev_key_state[controlScheme.rotate]) {
                    rotation_left *= -1;
                    fire_bullet();
                }
            } else if (DC->key_state[controlScheme.stun] && !DC->prev_key_state[controlScheme.stun]) {
                stun();
            } else {
                // 沒有按下 W 鍵，坦克停止移動
                hp_timer -= (1 / DC->FPS);
                if (hp_timer <= 0) {
                    if (hp <= (max_hp - 300)) hp += 300;
                    else hp = max_hp;
                    hp_timer = 3;
                }
                moving_forward = false;
            }
            break;
        };
        case TankState::DEAD: 
        {
            rotation_angle = 0;
            break;
        }
        case TankState::STOPPED: {
            speed = 0;
            if (DC->key_state[controlScheme.rotate] && !DC->prev_key_state[controlScheme.rotate])
                state = TankState::ALIVE;
            if (!DC->key_state[controlScheme.rotate]) moving_forward = false;
                break;
        }
        case TankState::STUNNED: {
            stun_timer -= (1 / DC->FPS);
            angular_speed = 1.0f;
            speed = 2.5;
            if (stun_timer <= 0) { state = TankState::ALIVE;}
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
            break;
        }
        case TankState::TANKSTATE_MAX: break; 
    }

    if (moving_forward == true) {
        float radian = rotation_angle;

        dx = speed * cos(radian);
        dy = speed * sin(radian);
        
        //tank won't go out
        if(position.x <= 0 && dx > 0){
            dx = 0;
        }
        if(position.x > 1575 && dx <= 0){
            dx = 0;
        }
        if(position.y <= 0 && dy > 0){
            dy = 0;
        }
        if(position.y >= 875 && dy <= 0){
            dy = 0;
        }
        
        if (!is_obstacle_overlap) {
            position.x -= dx;
            position.y -= dy;
            shape->update_center_x(shape->center_x() - dx);
            shape->update_center_y(shape->center_y() - dy);
        } else is_obstacle_overlap = false;

    } else {
        rotation_angle += (rotation_left * angular_speed);
        if (rotation_angle >= 2 * M_PI) {
            rotation_angle -= 2 * M_PI;
        } else if (rotation_angle < 0.0f) {
            rotation_angle += 2 * M_PI;
        }
    }
    for (auto it = DC->bullets.begin(); it != DC->bullets.end(); ) {
        if ((*it)->get_fly_dist() <= 0){
            it = DC->bullets.erase(it);
            //debug_log("AAAAAAA\n");
        }else ++it;
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

    if (state == TankState::DEAD) return;
    // for (auto &bullet : bullets) {
    //     bullet->draw();
    // }
    // 繪製血條
    float health_bar_width = 50.0;  // 血條的最大寬度
    float health_bar_height = 10.0; // 血條的高度
    float health_percentage = static_cast<float>(hp) / max_hp;

    float bar_x = position.x - 15; // 血條的左上角 X
    float bar_y = position.y - 30;                  // 血條的左上角 Y
    float bar_length = health_bar_width * health_percentage;

    float corner_radius = health_bar_height / 2;    // 圓角半徑

    ALLEGRO_COLOR health_color = al_map_rgb(255 * (1 - health_percentage), 255 * health_percentage, 0);

    // 繪製血條背景（灰色）
    al_draw_filled_rounded_rectangle(
        bar_x, bar_y, 
        bar_x + health_bar_width, bar_y + health_bar_height, 
        corner_radius, corner_radius, 
        al_map_rgb(100, 100, 100)
    );

    // 繪製血條（紅色）
    al_draw_filled_rounded_rectangle(
        bar_x, bar_y, 
        bar_x + bar_length, bar_y + health_bar_height, 
        corner_radius, corner_radius, 
        health_color
    );

    al_draw_rounded_rectangle(
    bar_x, bar_y, 
    bar_x + health_bar_width, bar_y + health_bar_height, 
    corner_radius, corner_radius, 
    al_map_rgb(100, 100, 100), 1
    );

    FontCenter *FC = FontCenter::get_instance();

    // 寫下寶石數量
    if (mode == 0) {
        al_draw_bitmap(IC->get("./assets/image/gem.png"), bar_x - 23, bar_y - 5, 0);
        al_draw_textf(
            FC->courier_new[FontSize::SMALL], al_map_rgb(0, 0, 0),
            bar_x - 13 , bar_y - 1,
            ALLEGRO_ALIGN_CENTRE, "%d", num_gem);
    }


    // 寫下血量
	al_draw_textf(
        FC->courier_new[FontSize::SMALL], al_map_rgb(0, 0, 0),
		bar_x + 25, bar_y - 6,
		ALLEGRO_ALIGN_CENTRE, "%d", hp);

    float bullet_width = 10.0f;          // 子彈條的寬度
    float bullet_height = 10.0f;         // 子彈條的高度
    float bullet_gap = 1.0f;            // 子彈條的間距
    float bullet_y = bar_y + 15;        // 子彈條的起始 Y 位置

    for (int i = 0; i < 6 + num_penerate; ++i) {
        float bullet_x = (bar_x - 7) + i * (bullet_width + bullet_gap); // 計算每個子彈條的 X 位置

        if (i < num_bullets) {
            // 裝備的子彈：綠色
            al_draw_filled_rounded_rectangle(
                bullet_x, bullet_y,
                bullet_x + bullet_width, bullet_y + bullet_height,
                corner_radius, corner_radius,
                al_map_rgb(205, 107, 52)
            );
        } else if (i < 6) {
            // 未裝備的子彈：灰色
            al_draw_filled_rounded_rectangle(
                bullet_x, bullet_y,
                bullet_x + bullet_width, bullet_y + bullet_height,
                corner_radius, corner_radius,
                al_map_rgb(100, 100, 100)
            );
        } else {
            // 穿透子彈：藍色
            al_draw_filled_rounded_rectangle(
                bullet_x, bullet_y,
                bullet_x + bullet_width, bullet_y + bullet_height,
                corner_radius, corner_radius,
                al_map_rgb(0, 0, 255)
            );
        }

        // 畫出邊框
        // al_draw_rounded_rectangle(
        //     bullet_x, bullet_y,
        //     bullet_x + bullet_width, bullet_y + bullet_height,
        //     corner_radius, corner_radius,
        //     al_map_rgb(0, 0, 0), 1.0f
        // );
    }

}