// Test the github
#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "Object.h"
#include <allegro5/bitmap.h>
#include <string>

class Bullet : public Object
{
public:
    Bullet(double x, double y, const float& angle ,int owner_id);
    void update();
    void draw();
    void set_fly_dist(double dist) { fly_dist = dist; }
    int get_owner_id() const { return owner_id; }
    bool is_out_of_range() const;
    const double &get_fly_dist() const { return fly_dist; }
    const int &get_dmg() const { return dmg; }
private:
    double vx;
    double vy;
    double fly_dist;
    double speed;
    float rotation_angle;
    int dmg;
    int owner_id;
    ALLEGRO_BITMAP *bitmap;
};

#endif
