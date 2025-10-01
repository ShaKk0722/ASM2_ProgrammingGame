#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class PauseMenuScene : public Scene {
private:
  SDL_Rect resumeButton, backButton;
  bool isResumeHovered, isBackHovered;
  TTF_Font *font;
  SDL_Rect musicSliderBar, musicSliderHandle;
  SDL_Rect sfxSliderBar, sfxSliderHandle;
  bool draggingMusic = false;
  bool draggingSfx = false;

public:
  PauseMenuScene();
  ~PauseMenuScene();
  void init(Manager *m) override;
  void handleEvents(SDL_Event event) override;
  void update() override;
  void render() override;
  void drawButton(SDL_Rect rect, const char *text, bool hovered);
  void drawText(const char *message, int x, int y, SDL_Color color,
                TTF_Font *font);
};
