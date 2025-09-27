#pragma once
#include "Scene.h"
#include "../objects/player.h"
#include "../objects/ball.h"
#include "../objects/ground.h"

class GamePlayWithAIScene : public Scene
{
private:
    Manager *manager;

    int fieldX, fieldY, fieldWidth, fieldHeight;
    int centerX, centerY, centerRadius;

    SDL_Rect leftGoal;
    SDL_Rect rightGoal;

    Player *team1Players[2];
    Player *team2Players[2];
    int activePlayer1 = 0;
    Ball *ball;
    Ground *ground;
    std::vector<SDL_Texture *> backgroundFrames;
    int currentFrame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 frameDelay = 1000 / 10;

    Uint32 lastAIMove = 0;

    void updateAI();
    void moveAIPlayer(int playerIndex);
    int findBestAIPlayer();
    void calculateAIMove(int playerIndex, int &moveX, int &moveY);

public:
    GamePlayWithAIScene();
    ~GamePlayWithAIScene();
    bool loadBackgroundFrames(const std::string &folder, int totalFrames);
    void init(Manager *m) override;
    void handleEvents(SDL_Event event) override;
    void update() override;
    void render() override;
};
