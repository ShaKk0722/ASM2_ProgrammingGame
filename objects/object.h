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
    float get_x() const;
    float get_y() const;
    float get_velocity_x() const;
    float get_velocity_y() const;
    float get_acceleration_x() const;
    float get_acceleration_y() const;
    float get_mass() const;
    float get_width() const;
    float get_height() const;
    float get_radius() const;
    void set_x(float x);
    void set_y(float y);
    void set_velocity(float velocity_x, float velocity_y);
    void set_acceleration(float acceleration_x, float acceleration_y);
    void set_mass(float mass);
    void set_shape(float width, float height, float radius);
};

#endif
