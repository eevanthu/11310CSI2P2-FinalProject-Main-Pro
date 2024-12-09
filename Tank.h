#ifndef TANK_H_INCLUDED
#define TANK_H_INCLUDED
#include <string>
#include <map>
#include "Object.h"

enum class TankState
{
    ALIVE,
    DEAD,
    TANKSTATE_MAX
};

class Tank : public Object
{
public:
    void init();
    void update();
    void draw();
    void fire_bullet();
private:
    TankState state = TankState::ALIVE; // the state of character
    double speed = 5;                   // the move speed of hero
    int width, height;                  // the width and height of the hero image
    std::map<TankState, std::string> pngPath;
    float rotation_angle = 0.0f;
    float angular_speed = 0.5;
    bool moving_forward = false;
    int rotation_left = 1;
};
#endif