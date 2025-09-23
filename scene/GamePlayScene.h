#pragma once
#include "Scene.h"

class GamePlayScene : public Scene
{
public:
    GamePlayScene();
    ~GamePlayScene();
    void init(Manager *m) override;
    void handleEvents(SDL_Event event) override;
};
