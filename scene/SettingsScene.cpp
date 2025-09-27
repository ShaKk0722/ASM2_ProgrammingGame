#include "SettingsScene.h"
#include "../Game.h"
#include <iostream>

SettingsScene::SettingsScene() {
  volume = MIX_MAX_VOLUME / 2; // start at 50%
  isDownHovered = false;
  isUpHovered = false;

  int buttonWidth = 100;
  int buttonHeight = 50;
  int screenWidth = 1000;
  int screenHeight = 600;
  int y = screenHeight / 2;
  int barWidth = 300;
  int barHeight = 10;
  int spacing = 40; // space between slider and buttons

  dragging = false;
  sliderBar = {(screenWidth - barWidth) / 2, (screenHeight * 2) / 5, barWidth,
               barHeight};

  sliderHandle = {sliderBar.x + (Game::volume * barWidth) / MIX_MAX_VOLUME - 10,
                  sliderBar.y - 10, 20, 30};

  volumeDownButton = {screenWidth / 2 - 150, sliderBar.y + 60 + spacing,
                      buttonWidth, buttonHeight};
  volumeUpButton = {screenWidth / 2 + 50, sliderBar.y + 60 + spacing,
                    buttonWidth, buttonHeight};
  backButton = {screenWidth / 2 - 50, volumeUpButton.y + buttonHeight + spacing,
                100, 50};
  isBackHovered = false;
}

SettingsScene::~SettingsScene() {
  if (font)
    TTF_CloseFont(font);
  if (backgroundTexture) {
    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = nullptr;
  }
}

void SettingsScene::init(Manager *m) {
  manager = m;
  SDL_Surface *surface = IMG_Load("assets/images/background_menu.jpg");
  if (surface) {
    backgroundTexture = SDL_CreateTextureFromSurface(Game::renderer, surface);
    SDL_FreeSurface(surface);
  } else {
    backgroundTexture = nullptr;
    std::cout << "Failed to load settings background: " << IMG_GetError()
              << std::endl;
  }

  font = TTF_OpenFont("assets/font/MadimiOne-Regular.ttf", 28);
  if (!font) {
    std::cout << "Failed to load font in settings: " << TTF_GetError()
              << std::endl;
  }
  std::cout << "SettingsScene initialized!" << std::endl;
}

void SettingsScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_MOUSEMOTION) {
    int mx = event.motion.x, my = event.motion.y;
    isDownHovered = (mx >= volumeDownButton.x &&
                     mx <= volumeDownButton.x + volumeDownButton.w &&
                     my >= volumeDownButton.y &&
                     my <= volumeDownButton.y + volumeDownButton.h);
    isUpHovered =
        (mx >= volumeUpButton.x && mx <= volumeUpButton.x + volumeUpButton.w &&
         my >= volumeUpButton.y && my <= volumeUpButton.y + volumeUpButton.h);
    isBackHovered = (mx >= backButton.x && mx <= backButton.x + backButton.w &&
                     my >= backButton.y && my <= backButton.y + backButton.h);

    // dragging logic
    if (dragging) {
      int newX = std::max(sliderBar.x, std::min(mx, sliderBar.x + sliderBar.w));
      sliderHandle.x = newX - sliderHandle.w / 2;
      Game::volume = ((newX - sliderBar.x) * MIX_MAX_VOLUME) / sliderBar.w;
      Mix_VolumeMusic(Game::volume);
    }
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    int mx = event.button.x, my = event.button.y;

    if (isDownHovered) {
      Game::volume = std::max(0, Game::volume - 1);
      Mix_VolumeMusic(Game::volume);
    }
    if (isUpHovered) {
      Game::volume = std::min(MIX_MAX_VOLUME, Game::volume + 1);
      Mix_VolumeMusic(Game::volume);
    }
    if (isBackHovered) {
      std::cout << "Back clicked!" << std::endl;
      Game::switchToMainMenu();
    }

    // check if click on slider handle
    if (mx >= sliderHandle.x && mx <= sliderHandle.x + sliderHandle.w &&
        my >= sliderHandle.y && my <= sliderHandle.y + sliderHandle.h) {
      dragging = true;
    }
  }

  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    dragging = false;
  }
}

void SettingsScene::update() {}

void SettingsScene::render() {
  // Draw background
  if (backgroundTexture) {
    SDL_RenderCopy(Game::renderer, backgroundTexture, nullptr, nullptr);
  } else {
    SDL_SetRenderDrawColor(Game::renderer, 50, 50, 50, 255);
    SDL_RenderClear(Game::renderer);
  }

  // === Slider Bar (rounded pill) ===
  SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
  SDL_Rect bar = {sliderBar.x, sliderBar.y, sliderBar.w, 20};
  SDL_RenderFillRect(Game::renderer, &bar);

  // === Slider Handle (circle knob) ===
  int cx = sliderHandle.x + sliderHandle.w / 2;
  int cy = sliderHandle.y + sliderHandle.h / 2;
  int radius = 12;

  SDL_SetRenderDrawColor(Game::renderer, 100, 149, 237, 255); // blue knob
  for (int w = -radius; w <= radius; w++) {
    for (int h = -radius; h <= radius; h++) {
      if (w * w + h * h <= radius * radius) {
        SDL_RenderDrawPoint(Game::renderer, cx + w, cy + h);
      }
    }
  }

  // === Buttons ===
  drawButton(volumeDownButton, "-", isDownHovered);
  drawButton(volumeUpButton, "+", isUpHovered);
  drawButton(backButton, "Back", isBackHovered);

  // === Volume Text ===
  SDL_Color black = {0, 0, 0, 255};
  char volText[64];
  sprintf(volText, "Volume: %d%%", (Game::volume * 100) / MIX_MAX_VOLUME);

  SDL_Surface *surf = TTF_RenderText_Blended(font, volText, black);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surf);

  SDL_Rect rect = {500 - surf->w / 2, sliderBar.y - 50, surf->w,
                   surf->h}; // place above slider
  SDL_FreeSurface(surf);
  SDL_RenderCopy(Game::renderer, tex, nullptr, &rect);
  SDL_DestroyTexture(tex);
}

void SettingsScene::drawButton(SDL_Rect rect, const char *text, bool hovered) {
  SDL_SetRenderDrawColor(Game::renderer, hovered ? 200 : 100, 100, 200, 255);
  SDL_RenderFillRect(Game::renderer, &rect);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface *surf = TTF_RenderText_Blended(font, text, white);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surf);

  SDL_Rect textRect = {rect.x + rect.w / 2 - surf->w / 2,
                       rect.y + rect.h / 2 - surf->h / 2, surf->w, surf->h};
  SDL_FreeSurface(surf);
  SDL_RenderCopy(Game::renderer, tex, nullptr, &textRect);
  SDL_DestroyTexture(tex);
}
