#include "ball.h"
#include "../Game.h"

Ball::Ball(float x, float y, float radius)
{
    set_x(x);
    set_y(y);
    set_shape(radius * 2, radius * 2, radius);
    this->velocity_x = 0;
    this->velocity_y = 0;
}

Ball::~Ball()
{
    if (texture) SDL_DestroyTexture(texture);
}



bool Ball::loadBall(const char* texturePath)
{
    SDL_Surface *surface = IMG_Load(texturePath);
    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);
        if (texture) {
            std::cout << "Ball image loaded successfully!" << std::endl;
            return true;
        } else {
            std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Failed to load ball image: " << IMG_GetError() << std::endl;
        texture = nullptr;
        return false;
    }
}



void Ball::render()
{
    if (texture) {
        SDL_Rect dst;
        dst.x = get_x() - get_radius();
        dst.y = get_y() - get_radius();
        dst.w = get_radius() * 2;
        dst.h = get_radius() * 2;
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    } else {
        // Fallback: draw orange circle
        SDL_SetRenderDrawColor(Game::renderer, 255, 165, 0, 255);
        int r = get_radius();
        int cx = get_x();
        int cy = get_y();
        for (int w = 0; w < r * 2; w++) {
            for (int h = 0; h < r * 2; h++) {
                int dx = r - w;
                int dy = r - h;
                if ((dx*dx + dy*dy) <= (r * r)) {
                    SDL_RenderDrawPoint(Game::renderer, cx + dx, cy + dy);
                }
            }
        }
    }
}

void Ball::move(float v_x, float v_y, int fieldX, int fieldY, int fieldWidth, int fieldHeight) {
    this->velocity_x = 0.995*this->velocity_x + v_x;
    this->velocity_y = 0.995*this->velocity_y + v_y;
    const float STOP_THRESHOLD = 0.000001f;
    if (std::abs(this->velocity_x) < STOP_THRESHOLD) {
        this->velocity_x = 0.0f;
    }
    if (std::abs(this->velocity_y) < STOP_THRESHOLD) {
        this->velocity_y = 0.0f;
    }
    float newX = x + this->velocity_x;
    float newY = y + this->velocity_y;

    // Clamp to field boundaries
    int minX = fieldX + radius;
    int maxX = fieldX + fieldWidth - radius;
    int minY = fieldY + radius;
    int maxY = fieldY + fieldHeight - radius;
    const float BOUNCE_FACTOR = 0.9f;

    if (newX < minX) {
        newX = minX;
        this->velocity_x = -this->velocity_x * BOUNCE_FACTOR; // Stop movement against the boundary
    }
    if (newX > maxX) {
        newX = maxX;
        this->velocity_x = -this->velocity_x * BOUNCE_FACTOR; // Stop movement against the boundary
    }
    if (newY < minY) {
        newY = minY;
        this->velocity_y = -this->velocity_y * BOUNCE_FACTOR; // Stop movement against the boundary
    }
    if (newY > maxY) {
        newY = maxY;
        this->velocity_y = -this->velocity_y * BOUNCE_FACTOR; // Stop movement against the boundary
    }

    set_x(newX);
    set_y(newY);
}