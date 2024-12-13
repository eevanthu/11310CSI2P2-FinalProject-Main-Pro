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
    DESTROYED,
    OBSTACLESTATE_MAX
};

class Obstacle : public Object
{

public:
    Obstacle(const Point &p);
    void init();
    void draw();
    void set_state();
    ObstacleState get_state() { return state; }

private:
    ObstacleState state; // the state of character
    // size and position
    int width, height;
    Point position;
    std::map<ObstacleState, std::string> pngPath;
};
#endif