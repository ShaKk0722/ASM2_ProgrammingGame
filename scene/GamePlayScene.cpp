#include "GamePlayScene.h"
#include <iostream>

GamePlayScene::GamePlayScene()
{
}

GamePlayScene::~GamePlayScene()
{
}

void GamePlayScene::init(Manager *m)
{
    manager = m;
    std::cout << "GamePlayScene initialized!" << std::endl;
}

void GamePlayScene::handleEvents(SDL_Event event)
{
    // Handle gameplay-specific events here
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_SPACE:
            std::cout << "GamePlay: Space pressed!" << std::endl;
            break;
        }
    }
}
