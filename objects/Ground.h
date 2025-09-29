#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "obstacle.h"
#include "../objects/player.h"
#include "../objects/ball.h"

class Ground {
private:
    int fieldX, fieldY, fieldWidth, fieldHeight;
    int centerX, centerY, centerRadius;
    SDL_Rect leftGoal;
    SDL_Rect rightGoal;
    SDL_Texture * texture = nullptr;

    std::vector<Obstacle*> blockingObstacles; // Obstacles that block movement
    std::vector<Obstacle*> slowingObstacles;  // Obstacles that slow down movement

public:
    Ground(int x, int y, int width, int height);
    ~Ground();

    void render();
    void update(Uint32 now); // remove expired obstacles

    int getCenterX() const { return centerX; }
    int getCenterY() const { return centerY; }
    int getFieldX() const { return fieldX; }
    int getFieldY() const { return fieldY; }
    int getFieldWidth() const { return fieldWidth; }
    int getFieldHeight() const { return fieldHeight; }
    bool loadGround(const char* texturePath);

    // Obstacle API
    void spawnInitialObstacles(int count, int minSize, int maxSize, const Ball* ball, Player** team1Players, Player** team2Players, int teamSize);
    void addBlockingObstacle(Obstacle* o);
    void addSlowingObstacle(Obstacle* o);
    void clearObstacles();

    // Queries used by Player/Ball
    bool isBlockedAt(int cx, int cy, int radius) const;
    float getSlowFactorAt(int cx, int cy, int radius) const;
};