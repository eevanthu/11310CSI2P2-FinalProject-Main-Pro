#ifndef OBSTACLE1_H_INCLUDED
#define OBSTACLE1_H_INCLUDED

#include <string>
#include <map>
#include "Object.h"
#include "shapes/Point.h"

enum class ObstacleState
{
    EXIST,
    ORANGE,
    BLUE,
    PINK,
    STONE,
    ITEM_MAXHP,
    ITEM_TANKSPEED,
    ITEM_NUMBULLET,
    ITEM_SHIELD,
    ITEM_PENETRATE,
    DESTROYED,
    GEM,
    OBSTACLESTATE_MAX
};

class Obstacle : public Object
{

public:
    Obstacle(const Point &p, ObstacleState state);
    void init();
    void draw();
    void set_state();
    ObstacleState get_state() { return state; }
    int mode = 0;

private:
    ObstacleState state; // the state of character
    // size and position
    int width, height;
    Point position;
    std::map<ObstacleState, std::string> pngPath;
};
#endif