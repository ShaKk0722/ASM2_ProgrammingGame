#include "ball.h"
#include "../Game.h"

Ball::Ball(float x, float y, float radius)
{
    set_x(x);
    set_y(y);
    set_shape(radius * 2, radius * 2, radius);
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