#include "ball.h"
#include "../Game.h"

Ball::Ball(float x, float y, float radius)
{
    set_x(x);
    set_y(y);
    set_shape(radius * 2, radius * 2, radius);
}

void Ball::render()
{
    SDL_SetRenderDrawColor(Game::renderer, 255, 165, 0, 255); // Orange ball (RGB: 255, 165, 0)
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