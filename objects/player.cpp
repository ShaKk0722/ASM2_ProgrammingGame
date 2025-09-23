
#include "player.h"

void Player::update()
{
    return;
}

void Player::render()
{
    return;
}

Player::Player(int x, int y, int width, int height, int radius, int team) : Object()
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->radius = radius;
    this->team = team;
}

Player::~Player()
{
    return;
}
