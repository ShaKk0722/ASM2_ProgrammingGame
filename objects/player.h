#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <SDL2/SDL.h>
#include "object.h"

class Player : public Object
{
private:
    int delta_position;
    float maxVel_X;
    float maxVel_Y;
    float delta_time;
    bool is_Choose;
    int team;

public:
    Player(int x, int y, int width, int height, int radius, int team);
    ~Player();
    void update();
    void render();
    void move(int dx, int dy, int fieldX, int fieldY, int fieldWidth, int fieldHeight);
};

#endif