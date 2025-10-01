#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

class SettingsScene : public Scene {
private:
  SDL_Rect volumeDownButton;
  SDL_Rect volumeUpButton;
  SDL_Rect backButton;
  SDL_Texture *backgroundTexture;
  SDL_Rect sliderBar;
  SDL_Rect sliderHandle;
  SDL_Rect sfxSliderBar;
  SDL_Rect sfxSliderHandle;

  SDL_Rect sfxDownButton;
  SDL_Rect sfxUpButton;
  bool isSfxDownHovered;
  bool isSfxUpHovered;
  bool dragging;
  bool draggingSfx;
  bool isDownHovered;
  bool isUpHovered;
  bool isBackHovered;

  TTF_Font *font;
  int volume; // 0–128 in SDL_mixer

public:
  SettingsScene();
  ~SettingsScene();
  void init(Manager *m) override;
  void handleEvents(SDL_Event event) override;
  void update() override;
  void render() override;
  void drawButton(SDL_Rect rect, const char *text, bool hovered);
  TTF_Font *getFont() const { return font; }
};
