#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class PauseMenuScene : public Scene {
private:
  SDL_Rect resumeButton, settingsButton, backButton;
  bool isResumeHovered, isSettingsHovered, isBackHovered;
  TTF_Font *font;

public:
  PauseMenuScene();
  ~PauseMenuScene();
  void init(Manager *m) override;
  void handleEvents(SDL_Event event) override;
  void update() override;
  void render() override;
  void drawButton(SDL_Rect rect, const char *text, bool hovered);
};
