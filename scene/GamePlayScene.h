#pragma once
#include "Scene.h"
#include "../objects/player.h"
#include "../objects/ball.h"
#include "../objects/ground.h"

class GamePlayScene : public Scene
{
private:
    Manager* manager;

    // Field properties
    int fieldX, fieldY, fieldWidth, fieldHeight;
    int centerX, centerY, centerRadius;

    // Goal properties
    SDL_Rect leftGoal;
    SDL_Rect rightGoal;

    Player* team1Players[2];
    Player* team2Players[2];
    int activePlayer1 = 0;
    int activePlayer2 = 0;
    Ball* ball;
    Ground* ground;
    std::vector<SDL_Texture*> backgroundFrames;
    int currentFrame = 0 ;
    Uint32 lastFrameTime = 0;
    Uint32 frameDelay = 1000 / 10;


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
    bool loadBackgroundFrames(const std::string& folder, int totalFrames);
    bool loadWindFrames(const std::string& folder, int totalFrames);
    void init(Manager* m);
    void handleEvents(SDL_Event event);
    void update();
    void render();
};
