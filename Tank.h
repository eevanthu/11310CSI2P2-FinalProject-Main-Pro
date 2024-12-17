#ifndef TANK_H_INCLUDED
#define TANK_H_INCLUDED
#include <string>
#include <map>
#include "Object.h"
#include "Bullet.h"
#include "shapes/Point.h"

enum class TankState
{
    ALIVE,
    TRIPLEBULLET,
    STOPPED,
    STUNNED,
    DEAD,
    TANKSTATE_MAX
};

struct ControlScheme {
    int rotate;
    int stun;
};


class Tank : public Object
{
public:
    Tank(const Point &p, const ControlScheme& controlScheme);
    void init();
    void update();
    void draw();
    void fire_bullet();
    void stun();
    void set_moving_forward(bool moving_forward) { this->moving_forward = moving_forward; }
    void set_position(const Point &p) {
        position.x = p.x - width / 2;
        position.y = p.y - height / 2;
        shape->update_center_x(p.x);
        shape->update_center_y(p.y); }
    void set_state(TankState state) { this->state = state; }
    void set_max_hp(float rate) { max_hp = max_hp * (1 + rate); }
    void set_speed() { if (speed <= 5) speed += 1; }
    void set_triple_timer() {triple_bullet_timer = 5;}
    void set_obstacle_overlap(bool is_overlap) { is_obstacle_overlap = is_overlap; }
    int set_num_shield(int num) { if (num_shield < 3) num_shield += num; return num_shield; }
    void set_num_penerate(int num) { num_penerate += num;}
    float get_rotation_angle() { return rotation_angle; }
    double get_position_x() { return position.x; }
    double get_position_y() { return position.y; }
    int get_id() const { return id; }
    TankState get_state() const { return state; }
    void decrease_hp(int demage) { if (hp > demage) this->hp -= demage;  else this->hp = 0; }
    int mode = 0; // 0: score mode, 1: kill mode
    int num_gem = 0;
    
private:
    // size, position and path
    Point position; // intitial position
    ControlScheme controlScheme;  
    TankState state = TankState::ALIVE; // the state of character
    float rotation_angle;
    int rotation_left;
    float angular_speed;
    double speed;
    bool moving_forward;
    bool is_obstacle_overlap;
    
    int hp;
    int max_hp;
    int num_bullets;
    int num_shield;
    int num_penerate = 0;
    int id;
    
    int width, height;                  // the width and height of the hero image
    std::map<TankState, std::string> pngPath;

    int dx = 0, dy = 0;

    float stun_timer = 0;
    float triple_bullet_timer = 0;
    float hp_timer = 3;
    float bullet_timer = 0.5;
};
#endif