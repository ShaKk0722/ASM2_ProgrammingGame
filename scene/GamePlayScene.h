#pragma once
#include "../objects/ball.h"
#include "../objects/ground.h"
#include "../objects/player.h"
#include "Scene.h"

class GamePlayScene : public Scene {
private:
  Manager *manager;

  // Field properties
  int fieldX, fieldY, fieldWidth, fieldHeight;
  int centerX, centerY, centerRadius;

  // Goal properties
  SDL_Point leftGoalTop;
  SDL_Point leftGoalBottom;
  SDL_Point rightGoalTop;
  SDL_Point rightGoalBottom;

  Player *team1Players[2];
  Player *team2Players[2];
  int activePlayer1 = 0;
  int activePlayer2 = 0;
  Ball *ball;
  Ground *ground;
  std::vector<SDL_Texture *> backgroundFrames;
  int currentFrame = 0;
  Uint32 lastFrameTime = 0;
  Uint32 frameDelay = 1000 / 10;

  // HaxBall features
  int team1Score = 0;
  int team2Score = 0;
  bool goalScored = false;
  Uint32 goalTime = 0;
  int goalTeam = 0; // Which team scored (1 or 2)
  bool ballResetPending = false;
  // In GamePlayScene.h (add private member)
  SDL_Rect pauseButton = {900, 20, 80, 40}; // top-right corner
  bool pauseHovered = false;
  // In GamePlayScene.h (and GamePlayWithAIScene.h)
  Uint32 startTime = 0;
  int elapsedSeconds = 0;
  bool matchOver = false;
  int winnerTeam = 0; // 1 or 2
  SDL_Rect againBtn = {380, 360, 240, 60};
  SDL_Rect backBtn = {380, 430, 240, 60};
  bool againHov = false, backHov = false;

  Uint32 lastNonActiveAIMove = 0;


    float windX; // Wind force in the X direction
    float windY; // Wind force in the Y direction

    std::vector<SDL_Texture*> windFrames; // Wind animation frames
    int currentWindFrame = 0;             // Current frame of the wind animation
    Uint32 lastWindFrameTime = 0;         // Time of the last wind frame update
    const Uint32 windFrameDelay = 100;    // Delay between wind frames (in milliseconds)

    // Wind effect timing
    bool windActive = false;             // Whether the wind effect is currently active
    Uint32 windStartTime = 0;            // When the wind effect started
    const Uint32 windDuration = 5000;    // Duration of the wind effect (in milliseconds)
    const Uint32 windCooldown = 10000;   // Cooldown before the wind effect reappears (in milliseconds)

public:
  GamePlayScene();
  ~GamePlayScene();
  bool loadBackgroundFrames(const std::string &folder, int totalFrames);
  bool loadWindFrames(const std::string& folder, int totalFrames);
  void init(Manager *m);
  void handleEvents(SDL_Event event);
  void update();
  void render();
  void reset();

  // HaxBall functions
  void checkGoal();
  void resetBall();
  void renderScore();
};
