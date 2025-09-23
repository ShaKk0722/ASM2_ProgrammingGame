#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>

class MainMenuScene : public Scene
{
private:
    SDL_Texture *backgroundTexture;
    SDL_Rect playWithPlayerButton;
    SDL_Rect playWithAIButton;
    bool isPlayWithPlayerHovered;
    bool isPlayWithAIHovered;

public:
    MainMenuScene();
    ~MainMenuScene();
    void init(Manager *m) override;
    void handleEvents(SDL_Event event) override;
    void update();
    void render();
    void loadBackground();
    bool isPointInRect(int x, int y, SDL_Rect rect);
};
