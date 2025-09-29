#pragma once
#include "Scene.h"
#include "../objects/player.h"
#include "../objects/ball.h"
#include "../objects/ground.h"

class GamePlayScene : public Scene
{
private:
    Manager *manager;

    // Field properties
    int fieldX, fieldY, fieldWidth, fieldHeight;
    int centerX, centerY, centerRadius;

    // Goal properties
    SDL_Rect leftGoal;
    SDL_Rect rightGoal;

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

public:
    GamePlayScene();
    ~GamePlayScene();
    bool loadBackgroundFrames(const std::string &folder, int totalFrames);
    void init(Manager *m);
    void handleEvents(SDL_Event event);
    void update();
    void render();

    // HaxBall functions
    void checkGoal();
    void resetBall();
    void renderScore();
};
