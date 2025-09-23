#include <iostream>
#include <SDL2/SDL.h>
#include "ECS.h"
#include "scene/Scene.h"
#include "scene/MainMenuScene.h"
#include "scene/GamePlayScene.h"
#include "scene/GamePlayWithAIScene.h"

enum class GameState
{
    MainMenu,
    GamePlay,
    GameOver,
    GamePlayWithAI
};

class Game
{
private:
    bool isRunning;
    SDL_Window *window;

public:
    static GameState gameState;

public:
    Game();
    ~Game();
    void init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; };
    static void switchToGamePlay();
    static void switchToGamePlayWithAI();
    static SDL_Renderer *renderer;
    static SDL_Event event;

    static Manager gameplayManager;
    static Manager mainMenuManager;
    static Manager gameOverManager;
    static Manager gamePlayWithAIManager;

    static MainMenuScene mainMenuScene;
    static GamePlayScene gameplayScene;
    static GamePlayWithAIScene gamePlayWithAIScene;
};
