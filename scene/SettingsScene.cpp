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

    isBackHovered = (mx >= backButton.x && mx <= backButton.x + backButton.w &&
                     my >= backButton.y && my <= backButton.y + backButton.h);

    // Dragging Music
    if (dragging) {
      int newX = std::max(sliderBar.x, std::min(mx, sliderBar.x + sliderBar.w));
      sliderHandle.x = newX - sliderHandle.w / 2;
      Game::volume = ((newX - sliderBar.x) * MIX_MAX_VOLUME) / sliderBar.w;
      Mix_VolumeMusic(Game::volume);
    }

    // Dragging SFX
    if (draggingSfx) {
      int newX = std::max(sfxSliderBar.x,
                          std::min(mx, sfxSliderBar.x + sfxSliderBar.w));
      sfxSliderHandle.x = newX - sfxSliderHandle.w / 2;
      Game::sfxVolume =
          ((newX - sfxSliderBar.x) * MIX_MAX_VOLUME) / sfxSliderBar.w;
      Mix_Volume(-1, Game::sfxVolume);
    }
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    int mx = event.button.x, my = event.button.y;

    // Music handle clicked
    if (mx >= sliderHandle.x && mx <= sliderHandle.x + sliderHandle.w &&
        my >= sliderHandle.y && my <= sliderHandle.y + sliderHandle.h) {
      dragging = true;
    }

    // SFX handle clicked
    if (mx >= sfxSliderHandle.x &&
        mx <= sfxSliderHandle.x + sfxSliderHandle.w &&
        my >= sfxSliderHandle.y &&
        my <= sfxSliderHandle.y + sfxSliderHandle.h) {
      draggingSfx = true;
    }

    // Back button
    if (isBackHovered) {
      if (Game::lastState == GameState::Pause) {
        Game::gameState = GameState::Pause;
      } else {
        Game::switchToMainMenu();
      }
    }
  }

  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    dragging = false;
    draggingSfx = false;
  }
}

void SettingsScene::update() {}

void SettingsScene::render() {
  // Background
  if (backgroundTexture) {
    SDL_RenderCopy(Game::renderer, backgroundTexture, nullptr, nullptr);
  } else {
    SDL_SetRenderDrawColor(Game::renderer, 50, 50, 50, 255);
    SDL_RenderClear(Game::renderer);
  }

  SDL_Color black = {0, 0, 0, 255};
  const int centerX = 500;

  // === Title ===
  {
    SDL_Surface *s = TTF_RenderText_Blended(font, "Settings", black);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, 60, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);
  }

  // === Music Section ===
  {
    // Label
    SDL_Surface *s = TTF_RenderText_Blended(font, "Music Volume", black);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, sliderBar.y - 40, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);

    // Bar + handle
    SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(Game::renderer, &sliderBar);
    SDL_SetRenderDrawColor(Game::renderer, 100, 149, 237, 255);
    SDL_RenderFillRect(Game::renderer, &sliderHandle);

    // Percentage
    char txt[16];
    std::snprintf(txt, sizeof(txt), "%d%%",
                  (Game::volume * 100) / MIX_MAX_VOLUME);
    SDL_Surface *p = TTF_RenderText_Blended(font, txt, black);
    SDL_Texture *pt = SDL_CreateTextureFromSurface(Game::renderer, p);
    SDL_Rect pr = {centerX - p->w / 2, sliderBar.y + 40, p->w, p->h};
    SDL_FreeSurface(p);
    SDL_RenderCopy(Game::renderer, pt, nullptr, &pr);
    SDL_DestroyTexture(pt);
  }

  // === SFX Section ===
  {
    // Label
    SDL_Surface *s = TTF_RenderText_Blended(font, "SFX Volume", black);
    SDL_Texture *t = SDL_CreateTextureFromSurface(Game::renderer, s);
    SDL_Rect r = {centerX - s->w / 2, sfxSliderBar.y - 40, s->w, s->h};
    SDL_FreeSurface(s);
    SDL_RenderCopy(Game::renderer, t, nullptr, &r);
    SDL_DestroyTexture(t);

    // Bar + handle
    SDL_SetRenderDrawColor(Game::renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(Game::renderer, &sfxSliderBar);
    SDL_SetRenderDrawColor(Game::renderer, 255, 165, 0, 255);
    SDL_RenderFillRect(Game::renderer, &sfxSliderHandle);

    // Percentage
    char txt[16];
    std::snprintf(txt, sizeof(txt), "%d%%",
                  (Game::sfxVolume * 100) / MIX_MAX_VOLUME);
    SDL_Surface *p = TTF_RenderText_Blended(font, txt, black);
    SDL_Texture *pt = SDL_CreateTextureFromSurface(Game::renderer, p);
    SDL_Rect pr = {centerX - p->w / 2, sfxSliderBar.y + 40, p->w, p->h};
    SDL_FreeSurface(p);
    SDL_RenderCopy(Game::renderer, pt, nullptr, &pr);
    SDL_DestroyTexture(pt);
  }

  // === Back Button ===
  backButton.x = centerX - backButton.w / 2;
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
