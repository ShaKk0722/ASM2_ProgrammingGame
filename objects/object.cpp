#include "object.h"

// Getter methods
float Object::get_x() const
{
    return x;
}

float Object::get_y() const
{
    return y;
}

float Object::get_velocity_x() const
{
    return velocity_x;
}

float Object::get_velocity_y() const
{
    return velocity_y;
}

float Object::get_acceleration_x() const
{
    return acceleration_x;
}

float Object::get_acceleration_y() const
{
    return acceleration_y;
}

float Object::get_mass() const
{
    return mass;
}

float Object::get_width() const
{
    return width;
}

float Object::get_height() const
{
    return height;
}

float Object::get_radius() const
{
    return radius;
}
// Setter methods
void Object::set_x(float x)
{
    this->x = x;
}

void Object::set_y(float y)
{
    this->y = y;
}

void Object::set_velocity(float velocity_x, float velocity_y)
{
    this->velocity_x = velocity_x;
    this->velocity_y = velocity_y;
}

void Object::set_acceleration(float acceleration_x, float acceleration_y)
{
    this->acceleration_x = acceleration_x;
    this->acceleration_y = acceleration_y;
}

void Object::set_mass(float mass)
{
    this->mass = mass;
}

void Object::set_shape(float width, float height, float radius)
{
    this->width = width;
    this->height = height;
    this->radius = radius;
}
