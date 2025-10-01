#include "PauseMenuScene.h"
#include "../Game.h"
#include <iostream>

PauseMenuScene::PauseMenuScene() {
  int buttonWidth = 250, buttonHeight = 60, spacing = 40;
  int screenWidth = 1000;
  int centerX = screenWidth / 2;

  // Start vertical position
  int startY = 200;

  // Buttons
  resumeButton = {centerX - buttonWidth / 2, startY, buttonWidth, buttonHeight};
  backButton = {centerX - buttonWidth / 2,
                sfxSliderBar.y + 100, // adjust this offset to taste
                buttonWidth, buttonHeight};

  isResumeHovered = isBackHovered = false;
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

  int screenWidth = 1000;
  int barWidth = 300, barHeight = 10;
  int centerX = screenWidth / 2;

  // Place sliders below Resume
  musicSliderBar = {centerX - barWidth / 2,
                    resumeButton.y + resumeButton.h + 50, barWidth, barHeight};
  musicSliderHandle = {musicSliderBar.x +
                           (Game::volume * barWidth) / MIX_MAX_VOLUME - 10,
                       musicSliderBar.y - 10, 20, 30};

  sfxSliderBar = {centerX - barWidth / 2,
                  musicSliderBar.y + 80, // space between sliders
                  barWidth, barHeight};
  sfxSliderHandle = {sfxSliderBar.x +
                         (Game::sfxVolume * barWidth) / MIX_MAX_VOLUME - 10,
                     sfxSliderBar.y - 10, 20, 30};
}

void PauseMenuScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_MOUSEMOTION && draggingMusic) {
    int newX =
        std::max(musicSliderBar.x,
                 std::min(event.motion.x, musicSliderBar.x + musicSliderBar.w));
    musicSliderHandle.x = newX - musicSliderHandle.w / 2;
    Game::volume =
        ((newX - musicSliderBar.x) * MIX_MAX_VOLUME) / musicSliderBar.w;
    Mix_VolumeMusic(Game::volume);
  }
  if (event.type == SDL_MOUSEMOTION && draggingSfx) {
    int newX =
        std::max(sfxSliderBar.x,
                 std::min(event.motion.x, sfxSliderBar.x + sfxSliderBar.w));
    sfxSliderHandle.x = newX - sfxSliderHandle.w / 2;
    Game::sfxVolume =
        ((newX - sfxSliderBar.x) * MIX_MAX_VOLUME) / sfxSliderBar.w;
    Mix_Volume(-1, Game::sfxVolume);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    if (event.button.x >= musicSliderHandle.x &&
        event.button.x <= musicSliderHandle.x + musicSliderHandle.w &&
        event.button.y >= musicSliderHandle.y &&
        event.button.y <= musicSliderHandle.y + musicSliderHandle.h)
      draggingMusic = true;

    if (event.button.x >= sfxSliderHandle.x &&
        event.button.x <= sfxSliderHandle.x + sfxSliderHandle.w &&
        event.button.y >= sfxSliderHandle.y &&
        event.button.y <= sfxSliderHandle.y + sfxSliderHandle.h)
      draggingSfx = true;
  }

  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    draggingMusic = draggingSfx = false;
  }

  if (event.type == SDL_MOUSEMOTION) {
    int mx = event.motion.x, my = event.motion.y;
    isResumeHovered =
        (mx >= resumeButton.x && mx <= resumeButton.x + resumeButton.w &&
         my >= resumeButton.y && my <= resumeButton.y + resumeButton.h);

    isBackHovered = (mx >= backButton.x && mx <= backButton.x + backButton.w &&
                     my >= backButton.y && my <= backButton.y + backButton.h);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    if (isResumeHovered)
      Game::resumeGame();

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
  drawButton(backButton, "Back to Menu", isBackHovered);

  // Restore blend mode if needed
  SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_NONE);

  // Draw Music Slider
  SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
  SDL_RenderFillRect(Game::renderer, &musicSliderBar);
  SDL_SetRenderDrawColor(Game::renderer, 100, 149, 237, 255);
  SDL_RenderFillRect(Game::renderer, &musicSliderHandle);
  drawText("Music", musicSliderBar.x + musicSliderBar.w / 2,
           musicSliderBar.y - 30, {255, 255, 255, 255}, font);

  // Draw SFX Slider
  SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
  SDL_RenderFillRect(Game::renderer, &sfxSliderBar);
  SDL_SetRenderDrawColor(Game::renderer, 255, 165, 0, 255);
  SDL_RenderFillRect(Game::renderer, &sfxSliderHandle);
  drawText("SFX", sfxSliderBar.x + sfxSliderBar.w / 2, sfxSliderBar.y - 30,
           {255, 255, 255, 255}, font);
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
void PauseMenuScene::drawText(const char *message, int x, int y,
                              SDL_Color color, TTF_Font *font) {
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
