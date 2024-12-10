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
        shape->update_center_x(p.x);
        shape->update_center_y(p.y); }
    void set_state(TankState state) { this->state = state; }
    void set_obstacle_overlap(bool is_overlap) { is_obstacle_overlap = is_overlap; }
    float get_rotation_angle() { return rotation_angle; }
    double get_position_x() { return position.x; }
    double get_position_y() { return position.y; }
    int get_id() const { return id; }
private:
    TankState state = TankState::ALIVE; // the state of character
    double speed;
    float rotation_angle;
    float angular_speed;
    bool moving_forward;
    int rotation_left;

    bool is_obstacle_overlap;

    int id;

    // size, position and path
    Point position;
    int width, height;                  // the width and height of the hero image
    std::map<TankState, std::string> pngPath;

    ControlScheme controlScheme;  

    float stun_timer = 0;
};
#endif