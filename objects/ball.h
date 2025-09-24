#ifndef __BALL_H__
#define __BALL_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "object.h"

class Ball : public Object
{
private:
    SDL_Texture* texture = nullptr;
public:
    Ball(float x, float y, float radius);
    ~Ball();
    bool loadBall(const char* texturePath);
    void render();
};

#endif