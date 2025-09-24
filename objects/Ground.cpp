#include "ground.h"
#include "../Game.h"

Ground::Ground(int x, int y, int width, int height)
    : fieldX(x), fieldY(y), fieldWidth(width), fieldHeight(height)
{
    centerX = fieldX + fieldWidth / 2;
    centerY = fieldY + fieldHeight / 2;
    centerRadius = 50;
    leftGoal = { fieldX - 10, centerY - 50, 10, 100 };
    rightGoal = { fieldX + fieldWidth, centerY - 50, 10, 100 };
}

void Ground::render()
{
    if (texture) {
        SDL_Rect dst = { fieldX, fieldY, fieldWidth, fieldHeight };
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    } else {
        // Fallback: draw green field and lines
        SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255);
        SDL_Rect fieldRect = { fieldX, fieldY, fieldWidth, fieldHeight };
        SDL_RenderFillRect(Game::renderer, &fieldRect);

        SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(Game::renderer, fieldX, fieldY + fieldHeight / 2,
                           fieldX + fieldWidth, fieldY + fieldHeight / 2);

        for (int w = 0; w < centerRadius * 2; w++) {
            for (int h = 0; h < centerRadius * 2; h++) {
                int dx = centerRadius - w;
                int dy = centerRadius - h;
                if ((dx*dx + dy*dy) <= (centerRadius * centerRadius)) {
                    SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
                }
            }
        }

        SDL_RenderFillRect(Game::renderer, &leftGoal);
        SDL_RenderFillRect(Game::renderer, &rightGoal);
    }
}

bool Ground::loadGround(const char* texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);
        if (texture) {
            std::cout << "Ground texture loaded successfully!" << std::endl;
            return true;
        } else {
            std::cout << "Failed to create ground texture: " << SDL_GetError() << std::endl;
            return false;
        }
    } else {
        std::cout << "Failed to load ground image: " << IMG_GetError() << std::endl;
        return false;
    }
}