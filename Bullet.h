// Test the github
#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "Object.h"
#include <allegro5/bitmap.h>
#include <string>

class Bullet : public Object
{
public:
    Bullet(double x, double y, const float& angle ,double speed);
    void update();
    void draw();
    void set_fly_dist(double dist) { fly_dist = dist; }
    bool is_out_of_range() const;
    const double &get_fly_dist() const { return fly_dist; }
    const int &get_dmg() const { return dmg; }
private:
    double vx;
    double vy;
    double fly_dist;
    float rotation_angle;
    int dmg;
    ALLEGRO_BITMAP *bitmap;
};

#endif
