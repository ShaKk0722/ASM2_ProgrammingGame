#pragma once
#include <SDL2/SDL.h>
#include "../ECS.h"
class Scene
{
public:
    Manager *manager;
    Scene() {}
    ~Scene() {}
    bool keyStates[SDL_NUM_SCANCODES] = { false };
    virtual void init(Manager *m) {};
    virtual void handleEvents(SDL_Event event) {};
    virtual void update() {};
    virtual void render() {};
};