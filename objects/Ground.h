#pragma once
#include <SDL2/SDL.h>

class Ground {
private:
    int fieldX, fieldY, fieldWidth, fieldHeight;
    int centerX, centerY, centerRadius;
    SDL_Rect leftGoal;
    SDL_Rect rightGoal;
    SDL_Texture * texture;

public:
    Ground(int x, int y, int width, int height);
    void render();
    int getFieldX() const { return fieldX; }
    int getFieldY() const { return fieldY; }
    int getFieldWidth() const { return fieldWidth; }
    int getFieldHeight() const { return fieldHeight; }
    int getCenterX() const { return centerX; }
    int getCenterY() const { return centerY; }
    bool loadGround(const char* texturePath);
};