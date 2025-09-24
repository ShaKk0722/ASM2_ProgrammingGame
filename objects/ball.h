#ifndef __BALL_H__
#define __BALL_H__

#include <SDL2/SDL.h>
#include "object.h"

class Ball : public Object
{
public:
    Ball(float x, float y, float radius);
    void render();
};

#endif