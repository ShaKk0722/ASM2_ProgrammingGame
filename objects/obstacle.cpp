#include "obstacle.h"
#include "../Game.h"
#include <SDL2/SDL_image.h>
#include <iostream>

Obstacle::Obstacle(int x, int y, int w, int h, Uint32 lifeMs, bool blocks_, float slow)
{
    // store as center-based like other objects
    set_x(x + w / 2.0f);
    set_y(y + h / 2.0f);
    set_shape((float)w, (float)h, std::min(w, h) / 2.0f);
    spawnTimeMs = SDL_GetTicks();
    lifetimeMs = lifeMs;
    blocks = blocks_;
    slowFactor = slow;
}

Obstacle::~Obstacle()
{
    if (texture) SDL_DestroyTexture(texture);
}

SDL_Rect Obstacle::getRect() const
{
    SDL_Rect r;
    r.w = (int)get_width();
    r.h = (int)get_height();
    r.x = (int)(get_x() - r.w / 2);
    r.y = (int)(get_y() - r.h / 2);
    return r;
}

bool Obstacle::loadTexture(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) {
        std::cout << "Obstacle IMG_Load failed: " << IMG_GetError() << std::endl;
        return false;
    }
    texture = SDL_CreateTextureFromSurface(Game::renderer, surf);
    SDL_FreeSurface(surf);
    if (!texture) {
        std::cout << "Obstacle CreateTexture failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void Obstacle::render()
{
    SDL_Rect dst = getRect();
    if (texture) {
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    } else {
        SDL_SetRenderDrawColor(Game::renderer, 200, 30, 30, 255);
        SDL_RenderFillRect(Game::renderer, &dst);
    }
}