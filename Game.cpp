#include "Game.h"

// Static member definitions
SDL_Renderer *Game::renderer = nullptr;
SDL_Event Game::event;
GameState Game::gameState = GameState::MainMenu;
Manager Game::gameplayManager;
Manager Game::mainMenuManager;
Manager Game::gameOverManager;
Manager Game::gamePlayWithAIManager;
Manager Game::settingsManager;

MainMenuScene Game::mainMenuScene;
GamePlayScene Game::gameplayScene;
GamePlayWithAIScene Game::gamePlayWithAIScene;
SettingsScene Game::settingsScene;

Mix_Music *Game::bgMusic = nullptr;
int Game::volume = MIX_MAX_VOLUME / 2;

Game::Game() {
  isRunning = false;
  window = nullptr;
  renderer = nullptr;
}

Game::~Game() { clean(); }

void Game::init(const char *title, int xpos, int ypos, int width, int height,
                bool fullscreen) {
  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
    std::cout << "SDL initialized successfully!" << std::endl;

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (window) {
      std::cout << "Window created!" << std::endl;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      std::cout << "Renderer created!" << std::endl;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
      std::cout << "SDL_image could not initialize! SDL_image Error: "
                << IMG_GetError() << std::endl;
      isRunning = false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
      std::cout << "SDL_mixer error: " << Mix_GetError() << std::endl;
    } else {
      initMusic();
    }

    // Initialize scenes
    mainMenuScene.init(&mainMenuManager);
    gameplayScene.init(&gameplayManager);
    gamePlayWithAIScene.init(&gamePlayWithAIManager);
    settingsScene.init(&settingsManager);

    isRunning = true;
  } else {
    isRunning = false;
  }
}

void Game::handleEvents() {
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    isRunning = false;
    break;

  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    case SDLK_ESCAPE:
      isRunning = false;
      break;
    case SDLK_1:
      gameState = GameState::MainMenu;
      std::cout << "Switched to Main Menu" << std::endl;
      break;
    case SDLK_2:
      gameState = GameState::GamePlay;
      std::cout << "Switched to GamePlay" << std::endl;
      break;
    case SDLK_3:
      gameState = GameState::GamePlayWithAI;
      std::cout << "Switched to GamePlay with AI" << std::endl;
      break;
    case SDLK_4:
      gameState = GameState::GameOver;
      std::cout << "Switched to Game Over" << std::endl;
      break;
    case SDLK_5: // shortcut to settings
      gameState = GameState::Settings;
      std::cout << "Switched to Settings" << std::endl;
      break;
    }
    break;

  default:
    break;
  }

  // Pass events to current scene
  switch (gameState) {
  case GameState::MainMenu:
    mainMenuScene.handleEvents(event);
    break;
  case GameState::GamePlay:
    gameplayScene.handleEvents(event);
    break;
  case GameState::GamePlayWithAI:
    gamePlayWithAIScene.handleEvents(event);
    break;
  case GameState::Settings:
    settingsScene.handleEvents(event);
    break;
  case GameState::GameOver:
    break;
  }
}

void Game::update() {
  switch (gameState) {
  case GameState::MainMenu:
    mainMenuManager.refresh();
    mainMenuManager.update();
    mainMenuScene.update();
    break;
  case GameState::GamePlay:
    gameplayScene.update();
    break;
  case GameState::GameOver:
    gameOverManager.refresh();
    gameOverManager.update();
    break;
  case GameState::GamePlayWithAI:
    gamePlayWithAIScene.update();
    break;
  case GameState::Settings:
    settingsManager.refresh();
    settingsManager.update();
    settingsScene.update();
    break;
  }
}

void Game::render() {
  SDL_RenderClear(renderer);

  switch (gameState) {
  case GameState::MainMenu:
    mainMenuScene.render();
    mainMenuManager.draw();
    break;
  case GameState::GamePlay:
    gameplayScene.render();
    gameplayManager.draw();
    break;
  case GameState::GameOver:
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    gameOverManager.draw();
    break;
  case GameState::GamePlayWithAI:
    gamePlayWithAIScene.render();
    gamePlayWithAIManager.draw();
    break;
  case GameState::Settings:
    settingsScene.render();
    settingsManager.draw();
    break;
  }

  SDL_RenderPresent(renderer);
}

void Game::clean() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  if (bgMusic) {
    Mix_FreeMusic(bgMusic);
    bgMusic = nullptr;
  }
  SDL_Quit();
  std::cout << "Game cleaned!" << std::endl;
}

void Game::switchToGamePlay() {
  gameState = GameState::GamePlay;
  std::cout << "Switched to GamePlay!" << std::endl;
}

void Game::switchToGamePlayWithAI() {
  gameState = GameState::GamePlayWithAI;
  std::cout << "Switched to GamePlay with AI!" << std::endl;
}

void Game::switchToSettings() {
  gameState = GameState::Settings;
  std::cout << "Switched to Settings!" << std::endl;
}

void Game::switchToMainMenu() {
  gameState = GameState::MainMenu;
  std::cout << "Switched to Main Menu!" << std::endl;
}

void Game::initMusic() {
  bgMusic = Mix_LoadMUS("assets/sounds/background.mp3");
  if (!bgMusic) {
    std::cout << "Failed to load bg music: " << Mix_GetError() << std::endl;
  } else {
    Mix_PlayMusic(bgMusic, -1); // loop forever
    Mix_VolumeMusic(volume);    // set starting volume
  }
}
