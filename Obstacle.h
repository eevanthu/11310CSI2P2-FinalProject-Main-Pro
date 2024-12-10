#ifndef OBSTACLE1_H_INCLUDED
#define OBSTACLE1_H_INCLUDED

#include <string>
#include <map>
#include "Object.h"
#include "shapes/Point.h"

enum class ObstacleState
{
    EXIST,
    DESTROYED,
    OBSTACLESTATE_MAX
};

class Obstacle : public Object
{

public:
    Obstacle(const Point &p);
    void init();
    void draw();

private:
    ObstacleState state = ObstacleState::EXIST; // the state of character
    // size and position
    int width, height;
    Point position;
    std::map<ObstacleState, std::string> pngPath;
};
#endif