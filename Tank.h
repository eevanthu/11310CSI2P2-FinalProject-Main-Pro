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
    void set_obstacle_overlap(bool is_overlap) { is_obstacle_overlap = is_overlap; }
    float get_rotation_angle() { return rotation_angle; }
    double get_position_x() { return position.x; }
    double get_position_y() { return position.y; }
    int get_id() const { return id; }
    void decrease_hp(int demage) { if (hp > demage) this->hp -= demage;  else this->hp = 0; }
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
    int id;
    
    int width, height;                  // the width and height of the hero image
    std::map<TankState, std::string> pngPath;

    float stun_timer = 0;
};
#endif