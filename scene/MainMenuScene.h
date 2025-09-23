#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>

class MainMenuScene : public Scene
{
private:
    SDL_Texture *backgroundTexture;
    SDL_Rect playWithPlayerButton;
    SDL_Rect playWithAIButton;
    SDL_Rect optionsButton;
    SDL_Rect exitButton;
    bool isPlayWithPlayerHovered;
    bool isPlayWithAIHovered;
    bool isOptionsHovered;
    bool isExitHovered;

public:
    MainMenuScene();
    ~MainMenuScene();
    void init(Manager *m) override;
    void handleEvents(SDL_Event event) override;
    void update();
    void render();
    void loadBackground();
    bool isPointInRect(int x, int y, SDL_Rect rect);
    void drawRoundedButton(SDL_Rect rect, int r, int g, int b, bool hovered);
};
