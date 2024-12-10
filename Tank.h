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
    void set_state(TankState state) { this->state = state; }
private:
    TankState state = TankState::ALIVE; // the state of character
    double speed;                   // the move speed of hero
    float rotation_angle;
    float angular_speed;
    bool moving_forward;
    int rotation_left;

    // size, position and path
    Point position;
    int width, height;                  // the width and height of the hero image
    std::map<TankState, std::string> pngPath;

    ControlScheme controlScheme;  

    float stun_timer = 0;
};
#endif