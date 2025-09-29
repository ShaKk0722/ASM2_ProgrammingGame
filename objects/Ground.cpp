#include "ground.h"
#include "../Game.h"

Ground::Ground(int x, int y, int width, int height)
    : fieldX(x), fieldY(y), fieldWidth(width), fieldHeight(height)
{
    centerX = fieldX + fieldWidth / 2;
    centerY = fieldY + fieldHeight / 2;
    centerRadius = 50;
    // Make goals larger and more prominent
    int goalWidth = 20;
    int goalHeight = 120;
    leftGoal = {fieldX - goalWidth, centerY - goalHeight / 2, goalWidth, goalHeight};
    rightGoal = {fieldX + fieldWidth, centerY - goalHeight / 2, goalWidth, goalHeight};
}

void Ground::render()
{
    if (texture)
    {
        SDL_Rect dst = {fieldX, fieldY, fieldWidth, fieldHeight};
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    }
    else
    {
        // Fallback: draw green field and lines
        SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255);
        SDL_Rect fieldRect = {fieldX, fieldY, fieldWidth, fieldHeight};
        SDL_RenderFillRect(Game::renderer, &fieldRect);

        SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(Game::renderer, fieldX, fieldY + fieldHeight / 2,
                           fieldX + fieldWidth, fieldY + fieldHeight / 2);

        for (int w = 0; w < centerRadius * 2; w++)
        {
            for (int h = 0; h < centerRadius * 2; h++)
            {
                int dx = centerRadius - w;
                int dy = centerRadius - h;
                if ((dx * dx + dy * dy) <= (centerRadius * centerRadius))
                {
                    SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }

    // Draw goal markers
    drawGoalMarkers();
}

void Ground::drawGoalMarkers()
{
    int markerRadius = 6;
    int borderWidth = 1;

    int leftMarkerX = leftGoal.x + leftGoal.w;
    int rightMarkerX = rightGoal.x;
    int topMarkerY = leftGoal.y;
    int bottomMarkerY = leftGoal.y + leftGoal.h;

    auto drawCircleWithBorder = [&](int centerX, int centerY, int r, int g, int b)
    {
        SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
        for (int w = 0; w < (markerRadius + borderWidth) * 2; w++)
        {
            for (int h = 0; h < (markerRadius + borderWidth) * 2; h++)
            {
                int dx = (markerRadius + borderWidth) - w;
                int dy = (markerRadius + borderWidth) - h;
                if ((dx * dx + dy * dy) <= ((markerRadius + borderWidth) * (markerRadius + borderWidth)))
                {
                    SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
                }
            }
        }

        // Draw colored circle inside (smaller circle)
        SDL_SetRenderDrawColor(Game::renderer, r, g, b, 255);
        for (int w = 0; w < markerRadius * 2; w++)
        {
            for (int h = 0; h < markerRadius * 2; h++)
            {
                int dx = markerRadius - w;
                int dy = markerRadius - h;
                if ((dx * dx + dy * dy) <= (markerRadius * markerRadius))
                {
                    SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
                }
            }
        }
    };

    drawCircleWithBorder(leftMarkerX, topMarkerY, 255, 0, 0);    // Top left corner
    drawCircleWithBorder(leftMarkerX, bottomMarkerY, 255, 0, 0); // Bottom left corner

    drawCircleWithBorder(rightMarkerX, topMarkerY, 0, 0, 255);    // Top right corner
    drawCircleWithBorder(rightMarkerX, bottomMarkerY, 0, 0, 255); // Bottom right corner
}

bool Ground::loadGround(const char *texturePath)
{
    SDL_Surface *surface = IMG_Load(texturePath);
    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);
        if (texture)
        {
            std::cout << "Ground texture loaded successfully!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "Failed to create ground texture: " << SDL_GetError() << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Failed to load ground image: " << IMG_GetError() << std::endl;
        return false;
    }
}