// #ifndef TANK_H_INCLUDED
// #define TANK_H_INCLUDED
// #include <string>
// #include <map>
// #include "Object.h"

// enum class TankState
// {
//     ALIVE,
//     DEAD
// };

// class Tank : public Object
// {
// public:
//     void init();
//     void update();
//     void draw();
// private:
//     TankState state = TankState::ALIVE; // the state of character
//     double speed = 5;                   // the move speed of hero
//     int width, height;                  // the width and height of the hero image
//     std::map<TankState, std::string> gifPath;
//     double rotation_angle = 0.0;
//     bool moving_forward = false;
// };
// #endif