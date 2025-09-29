#include "ground.h"
#include "../Game.h"
#include <SDL2/SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <math.h>

// helper: circle-rect intersection
static bool circleIntersectsRect(int cx, int cy, int r, const SDL_Rect& rect)
{
    int closestX = std::max(rect.x, std::min(cx, rect.x + rect.w));
    int closestY = std::max(rect.y, std::min(cy, rect.y + rect.h));
    int dx = cx - closestX;
    int dy = cy - closestY;
    return (dx * dx + dy * dy) <= (r * r);
}

Ground::Ground(int x, int y, int width, int height)
    : fieldX(x), fieldY(y), fieldWidth(width), fieldHeight(height)
{
    centerX = fieldX + fieldWidth / 2;
    centerY = fieldY + fieldHeight / 2;
    centerRadius = 50;
    leftGoal = { fieldX - 10, centerY - 50, 10, 100 };
    rightGoal = { fieldX + fieldWidth, centerY - 50, 10, 100 };

    // Seed RNG once
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

Ground::~Ground()
{
    if (texture) SDL_DestroyTexture(texture);
    clearObstacles();
}

void Ground::update(Uint32 now)
{
    // No periodic spawning or removal logic
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
                if ((dx * dx + dy * dy) <= (centerRadius * centerRadius)) {
                    SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
                }
            }
        }

        SDL_RenderFillRect(Game::renderer, &leftGoal);
        SDL_RenderFillRect(Game::renderer, &rightGoal);
    }

    // Draw blocking obstacles
    for (auto *o : blockingObstacles) o->render();

    // Draw slowing obstacles
    for (auto *o : slowingObstacles) o->render();
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

void Ground::spawnInitialObstacles(int count, int minSize, int maxSize, const Ball* ball, Player** team1Players, Player** team2Players, int teamSize)
{
    if (count <= 0) return;

    // Define center 1/3 area
    int regionX = fieldX + fieldWidth / 3;
    int regionY = fieldY + fieldHeight / 3;
    int regionW = fieldWidth / 3;
    int regionH = fieldHeight / 3;

    const int minDistance = 50; // Minimum distance from ball, players, and other obstacles

    int blockingCount = 0; // Counter for blocking obstacles
    int slowingCount = 0;  // Counter for slowing obstacles

    for (int i = 0; i < count; ++i) {
        int attempts = 100; // Limit the number of attempts to find a valid position
        bool placed = false;

        while (attempts-- > 0 && !placed) {
            int w = minSize;
            int h = minSize;
            int x = regionX + (std::rand() % std::max(1, regionW - w));
            int y = regionY + (std::rand() % std::max(1, regionH - h));

            SDL_Rect obstacleRect = { x, y, w, h };

            // Check for collisions with the ball
            SDL_Rect ballRect = { (int)(ball->get_x() - ball->get_radius()),
                                  (int)(ball->get_y() - ball->get_radius()),
                                  (int)(ball->get_radius() * 2),
                                  (int)(ball->get_radius() * 2) };

            if (SDL_HasIntersection(&obstacleRect, &ballRect)) continue;

            // Ensure obstacle is at least `minDistance` away from the ball
            int dx = x + w / 2 - (int)ball->get_x();
            int dy = y + h / 2 - (int)ball->get_y();
            if (std::sqrt(dx * dx + dy * dy) < minDistance) continue;

            // Check for collisions with team1 players
            bool collidesWithPlayer = false;
            for (int j = 0; j < teamSize; ++j) {
                SDL_Rect playerRect = { (int)(team1Players[j]->get_x() - team1Players[j]->get_radius()),
                                        (int)(team1Players[j]->get_y() - team1Players[j]->get_radius()),
                                        (int)(team1Players[j]->get_radius() * 2),
                                        (int)(team1Players[j]->get_radius() * 2) };

                if (SDL_HasIntersection(&obstacleRect, &playerRect)) {
                    collidesWithPlayer = true;
                    break;
                }

                // Ensure obstacle is at least `minDistance` away from the player
                int pdx = x + w / 2 - (int)team1Players[j]->get_x();
                int pdy = y + h / 2 - (int)team1Players[j]->get_y();
                if (std::sqrt(pdx * pdx + pdy * pdy) < minDistance) {
                    collidesWithPlayer = true;
                    break;
                }
            }

            // Check for collisions with team2 players
            for (int j = 0; j < teamSize; ++j) {
                SDL_Rect playerRect = { (int)(team2Players[j]->get_x() - team2Players[j]->get_radius()),
                                        (int)(team2Players[j]->get_y() - team2Players[j]->get_radius()),
                                        (int)(team2Players[j]->get_radius() * 2),
                                        (int)(team2Players[j]->get_radius() * 2) };

                if (SDL_HasIntersection(&obstacleRect, &playerRect)) {
                    collidesWithPlayer = true;
                    break;
                }

                // Ensure obstacle is at least `minDistance` away from the player
                int pdx = x + w / 2 - (int)team2Players[j]->get_x();
                int pdy = y + h / 2 - (int)team2Players[j]->get_y();
                if (std::sqrt(pdx * pdx + pdy * pdy) < minDistance) {
                    collidesWithPlayer = true;
                    break;
                }
            }

            if (collidesWithPlayer) continue;

            // Ensure obstacle is at least `minDistance` away from other obstacles
            bool collidesWithObstacle = false;
            for (const auto* existingObstacle : blockingObstacles) {
                int odx = x + w / 2 - (int)(existingObstacle->get_x());
                int ody = y + h / 2 - (int)(existingObstacle->get_y());
                if (std::sqrt(odx * odx + ody * ody) < minDistance) {
                    collidesWithObstacle = true;
                    break;
                }
            }
            for (const auto* existingObstacle : slowingObstacles) {
                int odx = x + w / 2 - (int)(existingObstacle->get_x());
                int ody = y + h / 2 - (int)(existingObstacle->get_y());
                if (std::sqrt(odx * odx + ody * ody) < minDistance) {
                    collidesWithObstacle = true;
                    break;
                }
            }

            if (collidesWithObstacle) continue;

            // Alternate between blocking and slowing obstacles to ensure 50% split
            bool blocking = false;
            if (blockingCount < slowingCount) {
                blocking = true;
                blockingCount++;
            } else {
                slowingCount++;
            }

            float slow = blocking ? 0.0f : (0.3f + (std::rand() % 71) / 100.0f);

            Obstacle* ob = new Obstacle(x, y, w, h, 0, blocking, slow); // Lifetime = 0 (permanent)

            if (blocking) {
                ob->loadTexture("assets/images/rock.png");
                addBlockingObstacle(ob);
            } else {
                ob->loadTexture("assets/images/mud.png");
                addSlowingObstacle(ob);
            }

            placed = true;
        }
    }
}

void Ground::addBlockingObstacle(Obstacle* o)
{
    if (o) blockingObstacles.push_back(o);
}

void Ground::addSlowingObstacle(Obstacle* o)
{
    if (o) slowingObstacles.push_back(o);
}

void Ground::clearObstacles()
{
    for (auto *o : blockingObstacles) delete o;
    for (auto *o : slowingObstacles) delete o;
    blockingObstacles.clear();
    slowingObstacles.clear();
}

bool Ground::isBlockedAt(int cx, int cy, int radius) const
{
    for (const auto *o : blockingObstacles) {
        if (o->isBlocking()) {
            if (circleIntersectsRect(cx, cy, radius, o->getRect())) return true;
        }
    }
    return false;
}

float Ground::getSlowFactorAt(int cx, int cy, int radius) const
{
    float factor = 1.0f;
    for (const auto *o : slowingObstacles) {
        if (circleIntersectsRect(cx, cy, radius, o->getRect())) {
            factor = std::min(factor, o->getSlowFactor());
            if (factor <= 0.0f) return 0.0f;
        }
    }
    return factor;
}