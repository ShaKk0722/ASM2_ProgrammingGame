#include "ECS.h"
#include "scene/GamePlayScene.h"
#include "scene/GamePlayWithAIScene.h"
#include "scene/MainMenuScene.h"
#include "scene/PauseMenuScene.h"
#include "scene/Scene.h"
#include "scene/SettingsScene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

enum class GameState {
  MainMenu,
  GamePlay,
  GamePlayWithAI,
  GameOver,
  Settings,
  Pause
};

class Game {
private:
  bool isRunning;
  SDL_Window *window;

public:
  static GameState gameState;

public:
  Game();
  ~Game();
  void init(const char *title, int xpos, int ypos, int width, int height,
            bool fullscreen);
  void handleEvents();
  void update();
  void render();
  void clean();
  bool running() { return isRunning; };
  static SettingsScene settingsScene;
  static Manager settingsManager;
  static void switchToSettings();
  static void switchToMainMenu();
  static void switchToGamePlay();
  static void switchToGamePlayWithAI();
  static void initMusic();
  static void resumeGame();
  static GameState lastState;
  static Manager pauseManager;
  static PauseMenuScene pauseMenuScene;

  static SDL_Renderer *renderer;
  static SDL_Event event;
  static Mix_Music *bgMusic;
  static int volume;
  static int sfxVolume;

  static bool sfxEnabled;
  static Mix_Chunk *kickSound;
  static Manager gameplayManager;
  static Manager mainMenuManager;
  static Manager gameOverManager;
  static Manager gamePlayWithAIManager;

  static MainMenuScene mainMenuScene;
  static GamePlayScene gameplayScene;
  static GamePlayWithAIScene gamePlayWithAIScene;
};
