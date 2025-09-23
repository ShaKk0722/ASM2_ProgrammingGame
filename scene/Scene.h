#pragma once
#include <SDL2/SDL.h>
#include "../ECS.h"
class Scene
{
public:
    Manager *manager;
    Scene() {}
    ~Scene() {}
    virtual void init(Manager *m) {};
    virtual void handleEvents(SDL_Event event) {};
    // virtual void update();
    // virtual void render();
};