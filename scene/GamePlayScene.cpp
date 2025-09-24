#include "GamePlayScene.h"
#include "../Game.h"
#include <iostream>

GamePlayScene::GamePlayScene()
    : fieldX(50), fieldY(50), fieldWidth(700), fieldHeight(400),
      centerX(400), centerY(250), centerRadius(50),
      leftGoal{40, 200, 10, 100}, rightGoal{750, 200, 10, 100}
{
    int playerRadius = 20;
    team1Players[0] = new Player(fieldX + 100, centerY - 50, playerRadius*2, playerRadius*2, playerRadius, 1);
    team1Players[1] = new Player(fieldX + 100, centerY + 50, playerRadius*2, playerRadius*2, playerRadius, 1);

    team2Players[0] = new Player(fieldX + fieldWidth - 100, centerY - 50, playerRadius*2, playerRadius*2, playerRadius, 2);
    team2Players[1] = new Player(fieldX + fieldWidth - 100, centerY + 50, playerRadius*2, playerRadius*2, playerRadius, 2);

    ball = new Ball(centerX, centerY, 15); // Ball at center
}

GamePlayScene::~GamePlayScene()
{
    delete team1Players[0];
    delete team1Players[1];
    delete team2Players[0];
    delete team2Players[1];
    delete ball;
}

void GamePlayScene::init(Manager *m)
{
    manager = m;
    std::cout << "GamePlayScene initialized!" << std::endl;
}


void GamePlayScene::handleEvents(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN && !event.key.repeat){
        keyStates[event.key.keysym.scancode] = true;
        // Team 1: TAB to switch player
        if (event.key.keysym.sym == SDLK_TAB)
            activePlayer1 = 1 - activePlayer1;

        // Team 2: ENTER to switch player
        if (event.key.keysym.sym == SDLK_RETURN)
            activePlayer2 = 1 - activePlayer2;
    }
    else if (event.type == SDL_KEYUP)
        keyStates[event.key.keysym.scancode] = false;
}

void GamePlayScene::update()
{
    int moveStep = 10;

    // Team 1 active player: WASD
    if (keyStates[SDL_SCANCODE_W])
        team1Players[activePlayer1]->move(0, -moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_S])
        team1Players[activePlayer1]->move(0, moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_A])
        team1Players[activePlayer1]->move(-moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_D])
        team1Players[activePlayer1]->move(moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);

    // Team 2 active player: Arrow keys
    if (keyStates[SDL_SCANCODE_UP])
        team2Players[activePlayer2]->move(0, -moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_DOWN])
        team2Players[activePlayer2]->move(0, moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_LEFT])
        team2Players[activePlayer2]->move(-moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_RIGHT])
        team2Players[activePlayer2]->move(moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);

}

void GamePlayScene::render()
{
    // Set field color (green)
    SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255); // Forest green
    SDL_Rect fieldRect = { fieldX, fieldY, fieldWidth, fieldHeight };
    SDL_RenderFillRect(Game::renderer, &fieldRect);

    // Draw field lines (white)
    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

    // Center line
    SDL_RenderDrawLine(Game::renderer, fieldX, fieldY + fieldHeight / 2,
                       fieldX + fieldWidth, fieldY + fieldHeight / 2);

    // Center circle
    for (int w = 0; w < centerRadius * 2; w++)
    {
        for (int h = 0; h < centerRadius * 2; h++)
        {
            int dx = centerRadius - w;
            int dy = centerRadius - h;
            if ((dx*dx + dy*dy) <= (centerRadius * centerRadius))
            {
                SDL_RenderDrawPoint(Game::renderer, centerX + dx, centerY + dy);
            }
        }
    }

    // Goals
    SDL_RenderFillRect(Game::renderer, &leftGoal);
    SDL_RenderFillRect(Game::renderer, &rightGoal);

    // Draw players
    for (int i = 0; i < 2; ++i) {
        // Highlight active player for Team 1
        if (i == activePlayer1) {
            SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 255); // Yellow border
            int borderRadius = team1Players[i]->get_radius() + 5;
            for (int w = 0; w < borderRadius * 2; w++) {
                for (int h = 0; h < borderRadius * 2; h++) {
                    int dx = borderRadius - w;
                    int dy = borderRadius - h;
                    int distSq = dx*dx + dy*dy;
                    if (distSq <= borderRadius * borderRadius && distSq >= (borderRadius-2)*(borderRadius-2)) {
                        SDL_RenderDrawPoint(Game::renderer, team1Players[i]->get_x() + dx, team1Players[i]->get_y() + dy);
                    }
                }
            }
        }
        team1Players[i]->render();

        // Highlight active player for Team 2
        if (i == activePlayer2) {
            SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 255); // Yellow border
            int borderRadius = team2Players[i]->get_radius() + 5;
            for (int w = 0; w < borderRadius * 2; w++) {
                for (int h = 0; h < borderRadius * 2; h++) {
                    int dx = borderRadius - w;
                    int dy = borderRadius - h;
                    int distSq = dx*dx + dy*dy;
                    if (distSq <= borderRadius * borderRadius && distSq >= (borderRadius-2)*(borderRadius-2)) {
                        SDL_RenderDrawPoint(Game::renderer, team2Players[i]->get_x() + dx, team2Players[i]->get_y() + dy);
                    }
                }
            }
        }
        team2Players[i]->render();
    }
    ball->render();
}

