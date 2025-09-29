#pragma once
#include "object.h"
#include <SDL2/SDL.h>

class Obstacle : public Object {
private:
    Uint32 spawnTimeMs = 0;
    Uint32 lifetimeMs = 0;
    bool blocks = false;
    float slowFactor = 1.0f;
    SDL_Texture* texture = nullptr;

public:
    Obstacle(int x, int y, int w, int h, Uint32 lifeMs = 5000, bool blocks = false, float slow = 1.0f);
    ~Obstacle();

    bool isExpired(Uint32 now) const { return (now - spawnTimeMs) >= lifetimeMs; }
    bool isBlocking() const { return blocks; }
    float getSlowFactor() const { return slowFactor; }
    SDL_Rect getRect() const;
    bool loadTexture(const char* path);
    void render();
};