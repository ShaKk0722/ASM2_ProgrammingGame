#include "PauseMenuScene.h"
#include "../Game.h"
#include <iostream>

PauseMenuScene::PauseMenuScene() {
  int buttonWidth = 250, buttonHeight = 60, spacing = 20;
  int screenWidth = 1000, screenHeight = 600;
  int totalHeight = 3 * buttonHeight + 2 * spacing;
  int startY = (screenHeight - totalHeight) / 2;

  resumeButton = {(screenWidth - buttonWidth) / 2, startY, buttonWidth,
                  buttonHeight};
  settingsButton = {(screenWidth - buttonWidth) / 2,
                    startY + buttonHeight + spacing, buttonWidth, buttonHeight};
  backButton = {(screenWidth - buttonWidth) / 2,
                startY + 2 * (buttonHeight + spacing), buttonWidth,
                buttonHeight};

  isResumeHovered = isSettingsHovered = isBackHovered = false;
}

PauseMenuScene::~PauseMenuScene() {
  if (font)
    TTF_CloseFont(font);
}

void PauseMenuScene::init(Manager *m) {
  manager = m;
  font = TTF_OpenFont("assets/font/MadimiOne-Regular.ttf", 28);
  if (!font)
    std::cout << "Pause menu font error: " << TTF_GetError() << std::endl;
}

void PauseMenuScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_MOUSEMOTION) {
    int mx = event.motion.x, my = event.motion.y;
    isResumeHovered =
        (mx >= resumeButton.x && mx <= resumeButton.x + resumeButton.w &&
         my >= resumeButton.y && my <= resumeButton.y + resumeButton.h);
    isSettingsHovered =
        (mx >= settingsButton.x && mx <= settingsButton.x + settingsButton.w &&
         my >= settingsButton.y && my <= settingsButton.y + settingsButton.h);
    isBackHovered = (mx >= backButton.x && mx <= backButton.x + backButton.w &&
                     my >= backButton.y && my <= backButton.y + backButton.h);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    if (isResumeHovered)
      Game::resumeGame();
    if (isSettingsHovered)
      Game::switchToSettings();
    if (isBackHovered)
      Game::switchToMainMenu();
  }
}

void PauseMenuScene::update() {}

void PauseMenuScene::render() {
  // Draw transparent overlay
  SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0,
                         128);          // 128 = half transparent
  SDL_Rect overlay = {0, 0, 1000, 800}; // your screen size
  SDL_RenderFillRect(Game::renderer, &overlay);

  // Draw buttons
  drawButton(resumeButton, "Resume", isResumeHovered);
  drawButton(settingsButton, "Settings", isSettingsHovered);
  drawButton(backButton, "Back to Menu", isBackHovered);

  // Restore blend mode if needed
  SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_NONE);
}

void PauseMenuScene::drawButton(SDL_Rect rect, const char *text, bool hovered) {
  SDL_SetRenderDrawColor(Game::renderer, hovered ? 200 : 100, 100, 200, 255);
  SDL_RenderFillRect(Game::renderer, &rect);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface *surf = TTF_RenderText_Blended(font, text, white);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surf);
  SDL_Rect txt = {rect.x + rect.w / 2 - surf->w / 2,
                  rect.y + rect.h / 2 - surf->h / 2, surf->w, surf->h};
  SDL_FreeSurface(surf);
  SDL_RenderCopy(Game::renderer, tex, nullptr, &txt);
  SDL_DestroyTexture(tex);
}
