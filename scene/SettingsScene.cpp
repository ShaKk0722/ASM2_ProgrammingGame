#include "SettingsScene.h"
#include "../Game.h"
#include <iostream>

SettingsScene::SettingsScene() {
  volume = MIX_MAX_VOLUME / 2; // start at 50%
  isDownHovered = isUpHovered = false;
  isSfxDownHovered = isSfxUpHovered = false;
  isBackHovered = false;

  int buttonWidth = 100;
  int buttonHeight = 50;
  int screenWidth = 1000;
  int screenHeight = 600;
  int barWidth = 300;
  int barHeight = 10;

  dragging = false;
  int centerX = screenWidth / 2;

  // Music slider section
  sliderBar = {centerX - barWidth / 2, 160, barWidth, barHeight};
  sliderHandle = {sliderBar.x + (Game::volume * barWidth) / MIX_MAX_VOLUME - 10,
                  sliderBar.y - 10, 20, 30};

  // SFX slider section (leave 120px gap after Music)
  sfxSliderBar = {centerX - barWidth / 2, sliderBar.y + 120, barWidth,
                  barHeight};
  sfxSliderHandle = {sfxSliderBar.x +
                         (Game::sfxVolume * barWidth) / MIX_MAX_VOLUME - 10,
                     sfxSliderBar.y - 10, 20, 30};

  // Back button at bottom (leave 150px after SFX)
  backButton = {centerX - 60, sfxSliderBar.y + 150, 120, 50};
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
      if (Game::lastState == GameState::Pause) {
        Game::gameState = GameState::Pause;
      } else {
        Game::switchToMainMenu();
      }
    }

    if (mx >= sliderHandle.x && mx <= sliderHandle.x + sliderHandle.w &&
        my >= sliderHandle.y && my <= sliderHandle.y + sliderHandle.h) {
      dragging = true;
    }
    if (isSfxDownHovered) {
      Game::sfxVolume = std::max(0, Game::sfxVolume - 1);
      Mix_Volume(-1, Game::sfxVolume);
    }
    if (isSfxUpHovered) {
      Game::sfxVolume = std::min(MIX_MAX_VOLUME, Game::sfxVolume + 1);
      Mix_Volume(-1, Game::sfxVolume);
    }
  }

  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    dragging = false;
  }
}

void SettingsScene::update() {}

void SettingsScene::render() {
  // --- Background ---
  if (backgroundTexture) {
    SDL_RenderCopy(Game::renderer, backgroundTexture, nullptr, nullptr);
  } else {
    SDL_SetRenderDrawColor(Game::renderer, 50, 50, 50, 255);
    SDL_RenderClear(Game::renderer);
  }

  // --- Colors ---
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color black = {0, 0, 0, 255};

  // Layout constants (tuned for 1000x600)
  const int centerX = 500;
  const int sectionGap = 120; // space between sections
  const int labelGap = 40;    // label above control
  const int belowGap = 40;    // text below control
  const int barHeight = 10;

  // ========== Title ==========
  {
    SDL_Surface *s = TTF_RenderText_Blended(font, "Settings", white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, 60, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // ********** MUSIC SECTION **********
  // Reposition music slider & handle for clean layout
  sliderBar.x = centerX - sliderBar.w / 2;
  sliderBar.y = 160; // bar Y
  // Handle follows current volume so +/- changes reflect visually
  sliderHandle.x = sliderBar.x +
                   ((Game::volume * sliderBar.w) / MIX_MAX_VOLUME) -
                   sliderHandle.w / 2;
  sliderHandle.y = sliderBar.y - (sliderHandle.h - barHeight) / 2 - 1;

  // Label: "Music Volume"
  {
    SDL_Surface *s = TTF_RenderText_Blended(font, "Music Volume", white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, sliderBar.y - labelGap, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // Slider bar + handle
  SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
  SDL_RenderFillRect(Game::renderer, &sliderBar);
  SDL_SetRenderDrawColor(Game::renderer, 100, 149, 237, 255);
  SDL_RenderFillRect(Game::renderer, &sliderHandle);

  // Music +/- buttons positioned under the slider
  volumeDownButton.x = centerX - 150;
  volumeDownButton.y = sliderBar.y + 20 + 10; // a bit below bar
  volumeUpButton.x = centerX + 50;
  volumeUpButton.y = volumeDownButton.y;
  drawButton(volumeDownButton, "-", isDownHovered);
  drawButton(volumeUpButton, "+", isUpHovered);

  // Music percent text centered below
  {
    char txt[16];
    std::snprintf(txt, sizeof(txt), "%d%%",
                  (Game::volume * 100) / MIX_MAX_VOLUME);
    SDL_Surface *s = TTF_RenderText_Blended(font, txt, white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, volumeDownButton.y + belowGap, s->w,
                  s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // ********** SFX SECTION **********
  const int sfxTopY = sliderBar.y + sectionGap; // start of SFX section

  // Label: "SFX Volume"
  {
    SDL_Surface *s = TTF_RenderText_Blended(font, "SFX Volume", white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, sfxTopY, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // SFX +/- buttons centered under the label
  sfxDownButton.x = centerX - 150;
  sfxDownButton.y = sfxTopY + 20;
  sfxUpButton.x = centerX + 50;
  sfxUpButton.y = sfxDownButton.y;
  drawButton(sfxDownButton, "SFX -", isSfxDownHovered);
  drawButton(sfxUpButton, "SFX +", isSfxUpHovered);

  // SFX percent text centered below its buttons
  {
    char txt[16];
    std::snprintf(txt, sizeof(txt), "%d%%",
                  (Game::sfxVolume * 100) / MIX_MAX_VOLUME);
    SDL_Surface *s = TTF_RenderText_Blended(font, txt, white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, sfxDownButton.y + belowGap, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // ********** BACK BUTTON **********
  backButton.x = centerX - backButton.w / 2;
  backButton.y = sfxDownButton.y + belowGap + 60; // nicely spaced under SFX %
  drawButton(backButton, "Back", isBackHovered);
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
