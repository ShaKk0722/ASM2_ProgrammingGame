#ifndef __OBJECT_H__
#define __OBJECT_H__

class Object
{
protected:
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float mass;
    float radius;
    float width;
    float height;

public:
    float get_x();
    float get_y();
    float get_velocity_x();
    float get_velocity_y();
    float get_acceleration_x();
    float get_acceleration_y();
    float get_mass();
    float get_width();
    float get_height();
    float get_radius();
    void set_x(float x);
    void set_y(float y);
    void set_velocity(float velocity_x, float velocity_y);
    void set_acceleration(float acceleration_x, float acceleration_y);
    void set_mass(float mass);
    void set_shape(float width, float height, float radius);
};

#endif
