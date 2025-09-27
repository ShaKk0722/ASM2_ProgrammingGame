#include "MainMenuScene.h"
#include "../Game.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

MainMenuScene::MainMenuScene() {
  backgroundTexture = nullptr;
  isPlayWithPlayerHovered = false;
  isPlayWithAIHovered = false;
  isOptionsHovered = false;
  isExitHovered = false;

  int buttonWidth = 320;
  int buttonHeight = 60;
  int screenWidth = 1000;
  int screenHeight = 600;
  int spacing = 20;

  int totalHeight = 4 * buttonHeight + 3 * spacing;
  int startY = (screenHeight - totalHeight) / 2;

  playWithPlayerButton = {(screenWidth - buttonWidth) / 2, startY, buttonWidth,
                          buttonHeight};

  playWithAIButton = {(screenWidth - buttonWidth) / 2,
                      startY + buttonHeight + spacing, buttonWidth,
                      buttonHeight};

  optionsButton = {(screenWidth - buttonWidth) / 2,
                   startY + 2 * (buttonHeight + spacing), buttonWidth,
                   buttonHeight};

  exitButton = {(screenWidth - buttonWidth) / 2,
                startY + 3 * (buttonHeight + spacing), buttonWidth,
                buttonHeight};
}

MainMenuScene::~MainMenuScene() {
  if (backgroundTexture) {
    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = nullptr;
  }
}

void MainMenuScene::init(Manager *m) {
  if (TTF_Init() == -1) {
    std::cout << "Failed to initialize SDL_ttf: " << TTF_GetError()
              << std::endl;
  }
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cout << "SDL_mixer could not initialize! SDL_mixer Error: "
              << Mix_GetError() << std::endl;
  }

  font = TTF_OpenFont("assets/font/MadimiOne-Regular.ttf", 28);
  if (!font) {
    std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
  }

  manager = m;
  loadBackground();
  std::cout << "MainMenuScene initialized!" << std::endl;
}

void MainMenuScene::loadBackground() {
  // // Initialize SDL_image
  // if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
  // {
  //     std::cout << "SDL_image could not initialize! SDL_image Error: " <<
  //     IMG_GetError() << std::endl; backgroundTexture = nullptr; return;
  // }

  // Load the background image
  SDL_Surface *surface = IMG_Load("assets/images/background_menu.jpg");
  if (surface) {
    backgroundTexture = SDL_CreateTextureFromSurface(Game::renderer, surface);
    SDL_FreeSurface(surface);
    std::cout << "Background image loaded successfully!" << std::endl;
  } else {
    std::cout << "Failed to load background image: " << IMG_GetError()
              << std::endl;
    backgroundTexture = nullptr;
  }
}

void MainMenuScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_MOUSEMOTION) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;

    isPlayWithPlayerHovered =
        isPointInRect(mouseX, mouseY, playWithPlayerButton);
    isPlayWithAIHovered = isPointInRect(mouseX, mouseY, playWithAIButton);
    isOptionsHovered = isPointInRect(mouseX, mouseY, optionsButton);
    isExitHovered = isPointInRect(mouseX, mouseY, exitButton);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    int mouseX = event.button.x;
    int mouseY = event.button.y;

    if (isPointInRect(mouseX, mouseY, playWithPlayerButton)) {
      std::cout << "Play With Players clicked!" << std::endl;
      Game::switchToGamePlay();
    }

    if (isPointInRect(mouseX, mouseY, playWithAIButton)) {
      std::cout << "Play With AI clicked!" << std::endl;
      Game::switchToGamePlayWithAI();
    }

    if (isPointInRect(mouseX, mouseY, optionsButton)) {
      std::cout << "Options clicked!" << std::endl;
      Game::switchToSettings();
    }

    if (isPointInRect(mouseX, mouseY, exitButton)) {
      std::cout << "Exit clicked!" << std::endl;
      exit(0);
    }
  }
}

void MainMenuScene::update() {}

void MainMenuScene::render() {
  SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255);
  SDL_RenderClear(Game::renderer);

  if (backgroundTexture) {
    SDL_RenderCopy(Game::renderer, backgroundTexture, nullptr, nullptr);
  } else {

    SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255);
    SDL_RenderClear(Game::renderer);

    SDL_SetRenderDrawColor(Game::renderer, 100, 100, 100, 255);
    SDL_Rect topStand = {0, 0, 1000, 100};
    SDL_Rect bottomStand = {0, 500, 1000, 100};
    SDL_RenderFillRect(Game::renderer, &topStand);
    SDL_RenderFillRect(Game::renderer, &bottomStand);

    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
    SDL_Rect centerLine = {495, 100, 10, 400};
    SDL_RenderFillRect(Game::renderer, &centerLine);
  }

  drawRoundedButton(playWithPlayerButton, 255, 140, 0, isPlayWithPlayerHovered);
  drawRoundedButton(playWithAIButton, 30, 144, 255, isPlayWithAIHovered);
  drawRoundedButton(optionsButton, 200, 200, 200, isOptionsHovered);
  drawRoundedButton(exitButton, 255, 99, 99, isExitHovered);
  SDL_Color white = {255, 255, 255, 255};

  drawText("Play With Player",
           playWithPlayerButton.x + playWithPlayerButton.w / 2,
           playWithPlayerButton.y + playWithPlayerButton.h / 2, white, font);

  drawText("Play With AI", playWithAIButton.x + playWithAIButton.w / 2,
           playWithAIButton.y + playWithAIButton.h / 2, white, font);

  drawText("Options", optionsButton.x + optionsButton.w / 2,
           optionsButton.y + optionsButton.h / 2, white, font);

  drawText("Exit", exitButton.x + exitButton.w / 2,
           exitButton.y + exitButton.h / 2, white, font);
}

bool MainMenuScene::isPointInRect(int x, int y, SDL_Rect rect) {
  return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y &&
          y <= rect.y + rect.h);
}

void MainMenuScene::drawRoundedButton(SDL_Rect rect, int r, int g, int b,
                                      bool hovered) {
  if (hovered) {
    r = (r + 30 > 255) ? 255 : r + 30;
    g = (g + 30 > 255) ? 255 : g + 30;
    b = (b + 30 > 255) ? 255 : b + 30;
  }

  int radius = 30;

  SDL_SetRenderDrawColor(Game::renderer, r, g, b, 255);

  SDL_Rect centerRect = {rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
  SDL_RenderFillRect(Game::renderer, &centerRect);

  SDL_Rect leftRect = {rect.x, rect.y + radius, radius, rect.h - 2 * radius};
  SDL_RenderFillRect(Game::renderer, &leftRect);

  SDL_Rect rightRect = {rect.x + rect.w - radius, rect.y + radius, radius,
                        rect.h - 2 * radius};
  SDL_RenderFillRect(Game::renderer, &rightRect);

  for (int x = -radius; x <= 0; x++) {
    for (int y = -radius; y <= 0; y++) {
      if (x * x + y * y <= radius * radius) {
        SDL_RenderDrawPoint(Game::renderer, rect.x + radius + x,
                            rect.y + radius + y);
      }
    }
  }

  for (int x = 0; x <= radius; x++) {
    for (int y = -radius; y <= 0; y++) {
      if (x * x + y * y <= radius * radius) {
        SDL_RenderDrawPoint(Game::renderer, rect.x + rect.w - radius + x,
                            rect.y + radius + y);
      }
    }
  }

  for (int x = -radius; x <= 0; x++) {
    for (int y = 0; y <= radius; y++) {
      if (x * x + y * y <= radius * radius) {
        SDL_RenderDrawPoint(Game::renderer, rect.x + radius + x,
                            rect.y + rect.h - radius + y);
      }
    }
  }

  for (int x = 0; x <= radius; x++) {
    for (int y = 0; y <= radius; y++) {
      if (x * x + y * y <= radius * radius) {
        SDL_RenderDrawPoint(Game::renderer, rect.x + rect.w - radius + x,
                            rect.y + rect.h - radius + y);
      }
    }
  }
}
void MainMenuScene::drawText(const char *message, int x, int y, SDL_Color color,
                             TTF_Font *font) {
  SDL_Surface *surface = TTF_RenderText_Blended(font, message, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, surface);

  SDL_Rect textRect;
  textRect.x = x - surface->w / 2; // center horizontally
  textRect.y = y - surface->h / 2; // center vertically
  textRect.w = surface->w;
  textRect.h = surface->h;

  SDL_FreeSurface(surface);
  SDL_RenderCopy(Game::renderer, texture, nullptr, &textRect);
  SDL_DestroyTexture(texture);
}
