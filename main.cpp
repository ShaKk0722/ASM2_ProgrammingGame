#include "Game.h"

Game *game = nullptr;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const int FPS = 60;
const int deltaTime = 1000 / FPS;

int main(int argc, char *argv[])
{
    Uint32 frameStart;
    int frameTime;
    game = new Game();
    game->init("2D_SOCCER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false);
    while (game->running())
    {
        frameStart = SDL_GetTicks();
        game->handleEvents();
        game->update();
        game->render();
    }
    game->clean();
    return 0;
}