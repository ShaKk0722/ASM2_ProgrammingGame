
#include "player.h"
#include "../Game.h"

void Player::update()
{
    return;
}

void Player::render()
{
    SDL_Color color = (team == 1) ? SDL_Color{0, 0, 255, 255} : SDL_Color{255, 0, 0, 255};
    SDL_SetRenderDrawColor(Game::renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(Game::renderer, x + dx, y + dy);
            }
        }
    }
}

Player::Player(int x, int y, int width, int height, int radius, int team) : Object()
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->radius = radius;
    this->team = team;
}

Player::~Player()
{
    return;
}


void Player::move(int dx, int dy, int fieldX, int fieldY, int fieldWidth, int fieldHeight)
{
    int newX = x + dx;
    int newY = y + dy;

    // Clamp to field boundaries
    int minX = fieldX + radius;
    int maxX = fieldX + fieldWidth - radius;
    int minY = fieldY + radius;
    int maxY = fieldY + fieldHeight - radius;

    if (newX < minX) newX = minX;
    if (newX > maxX) newX = maxX;
    if (newY < minY) newY = minY;
    if (newY > maxY) newY = maxY;

    x = newX;
    y = newY;
}