#include "object.h"

// Getter methods
float Object::get_x()
{
    return x;
}

float Object::get_y()
{
    return y;
}

float Object::get_velocity_x()
{
    return velocity_x;
}

float Object::get_velocity_y()
{
    return velocity_y;
}

float Object::get_acceleration_x()
{
    return acceleration_x;
}

float Object::get_acceleration_y()
{
    return acceleration_y;
}

float Object::get_mass()
{
    return mass;
}

float Object::get_width()
{
    return width;
}

float Object::get_height()
{
    return height;
}

float Object::get_radius()
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
