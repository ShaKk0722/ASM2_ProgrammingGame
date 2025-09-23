#pragma once
#include "Scene.h"

class GamePlayWithAIScene : public Scene
{
public:
    GamePlayWithAIScene();
    ~GamePlayWithAIScene();
    void init(Manager *m) override;
    void handleEvents(SDL_Event event) override;
};
