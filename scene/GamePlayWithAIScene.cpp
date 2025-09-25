#include "GamePlayWithAIScene.h"
#include "../Game.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GamePlayWithAIScene::GamePlayWithAIScene()
{
    const int groundWidth = 800;
    const int groundHeight = 500;
    const int groundX = (1000 - groundWidth) / 2;
    const int groundY = (800 - groundHeight) / 2;

    ground = new Ground(groundX, groundY, groundWidth, groundHeight);
    int playerRadius = 20;
    fieldX = ground->getFieldX();
    fieldY = ground->getFieldY();
    fieldWidth = ground->getFieldWidth();
    fieldHeight = ground->getFieldHeight();
    centerX = ground->getCenterX();
    centerY = ground->getCenterY();

    // Team 1: Human players
    team1Players[0] = new Player(fieldX + 100, centerY - 50, playerRadius * 2, playerRadius * 2, playerRadius, 1);
    team1Players[1] = new Player(fieldX + 100, centerY + 50, playerRadius * 2, playerRadius * 2, playerRadius, 1);

    // Team 2: AI players
    team2Players[0] = new Player(fieldX + fieldWidth - 100, centerY - 50, playerRadius * 2, playerRadius * 2, playerRadius, 2);
    team2Players[1] = new Player(fieldX + fieldWidth - 100, centerY + 50, playerRadius * 2, playerRadius * 2, playerRadius, 2);

    ball = new Ball(centerX, centerY, 15);
}

GamePlayWithAIScene::~GamePlayWithAIScene()
{
    delete ground;
    delete team1Players[0];
    delete team1Players[1];
    delete team2Players[0];
    delete team2Players[1];
    delete ball;
}

bool GamePlayWithAIScene::loadBackgroundFrames(const std::string &folder, int totalFrames)
{
    for (int i = 1; i <= totalFrames; i++)
    {
        char path[256];
        sprintf(path, "%s/frame_%03d.png", folder.c_str(), i);

        SDL_Surface *surface = IMG_Load(path);
        if (!surface)
        {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << std::endl;
            continue;
        }
        SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);

        if (tex)
        {
            backgroundFrames.push_back(tex);
        }
    }

    return !backgroundFrames.empty();
}

void GamePlayWithAIScene::init(Manager *m)
{
    manager = m;
    ball->loadBall("assets/images/ball.png");
    team1Players[0]->loadPlayer("assets/images/vietnam.png");
    team1Players[1]->loadPlayer("assets/images/vietnam.png");
    team2Players[0]->loadPlayer("assets/images/china.png");
    team2Players[1]->loadPlayer("assets/images/china.png");
    ground->loadGround("assets/images/football_field.jpeg");
    this->loadBackgroundFrames("assets/images/cheering_6", 10);
    std::cout << "GamePlayWithAIScene initialized!" << std::endl;
}

void GamePlayWithAIScene::handleEvents(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN && !event.key.repeat)
    {
        keyStates[event.key.keysym.scancode] = true;
        if (event.key.keysym.sym == SDLK_TAB)
            activePlayer1 = 1 - activePlayer1;
    }
    else if (event.type == SDL_KEYUP)
        keyStates[event.key.keysym.scancode] = false;
}

void GamePlayWithAIScene::update()
{
    int moveStep = 5;

    if (keyStates[SDL_SCANCODE_W])
        team1Players[activePlayer1]->move(0, -moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_S])
        team1Players[activePlayer1]->move(0, moveStep, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_A])
        team1Players[activePlayer1]->move(-moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);
    if (keyStates[SDL_SCANCODE_D])
        team1Players[activePlayer1]->move(moveStep, 0, fieldX, fieldY, fieldWidth, fieldHeight);

    updateAI();

    Uint32 now = SDL_GetTicks();
    if (now > lastFrameTime + frameDelay)
    {
        currentFrame = (currentFrame + 1) % backgroundFrames.size();
        lastFrameTime = now;
    }
}

void GamePlayWithAIScene::render()
{
    if (!backgroundFrames.empty())
    {
        SDL_RenderCopy(Game::renderer, backgroundFrames[currentFrame], NULL, NULL);
    }
    else
    {
        std::cout << "No background texture loaded!" << std::endl;
    }
    ground->render();

    for (int i = 0; i < 2; ++i)
    {
        if (i == activePlayer1)
        {
            SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 255); // Yellow border
            int borderRadius = team1Players[i]->get_radius() + 5;
            for (int w = 0; w < borderRadius * 2; w++)
            {
                for (int h = 0; h < borderRadius * 2; h++)
                {
                    int dx = borderRadius - w;
                    int dy = borderRadius - h;
                    int distSq = dx * dx + dy * dy;
                    if (distSq <= borderRadius * borderRadius && distSq >= (borderRadius - 2) * (borderRadius - 2))
                    {
                        SDL_RenderDrawPoint(Game::renderer, team1Players[i]->get_x() + dx, team1Players[i]->get_y() + dy);
                    }
                }
            }
        }
        team1Players[i]->render();
    }

    for (int i = 0; i < 2; ++i)
    {
        int bestAIPlayer = findBestAIPlayer();
        if (i == bestAIPlayer)
        {
            SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, 255); // Red border for AI
            int borderRadius = team2Players[i]->get_radius() + 5;
            for (int w = 0; w < borderRadius * 2; w++)
            {
                for (int h = 0; h < borderRadius * 2; h++)
                {
                    int dx = borderRadius - w;
                    int dy = borderRadius - h;
                    int distSq = dx * dx + dy * dy;
                    if (distSq <= borderRadius * borderRadius && distSq >= (borderRadius - 2) * (borderRadius - 2))
                    {
                        SDL_RenderDrawPoint(Game::renderer, team2Players[i]->get_x() + dx, team2Players[i]->get_y() + dy);
                    }
                }
            }
        }
        team2Players[i]->render();
    }
    ball->render();
}

void GamePlayWithAIScene::updateAI()
{
    Uint32 now = SDL_GetTicks();

    if (now - lastAIMove >= 300)
    {
        int bestAIPlayer = findBestAIPlayer();
        moveAIPlayer(bestAIPlayer);
        lastAIMove = now;
    }
}

void GamePlayWithAIScene::moveAIPlayer(int playerIndex)
{
    if (playerIndex < 0 || playerIndex >= 2)
        return;

    int moveX = 0, moveY = 0;
    calculateAIMove(playerIndex, moveX, moveY);

    team2Players[playerIndex]->move(moveX, moveY, fieldX, fieldY, fieldWidth, fieldHeight);
}

int GamePlayWithAIScene::findBestAIPlayer()
{

    float ballX = ball->get_x();
    float ballY = ball->get_y();

    float dist0 = sqrt(pow(team2Players[0]->get_x() - ballX, 2) + pow(team2Players[0]->get_y() - ballY, 2));
    float dist1 = sqrt(pow(team2Players[1]->get_x() - ballX, 2) + pow(team2Players[1]->get_y() - ballY, 2));

    return (dist0 < dist1) ? 0 : 1;
}

void GamePlayWithAIScene::calculateAIMove(int playerIndex, int &moveX, int &moveY)
{

    float ballX = ball->get_x();
    float ballY = ball->get_y();
    float playerX = team2Players[playerIndex]->get_x();
    float playerY = team2Players[playerIndex]->get_y();

    float dx = ballX - playerX;
    float dy = ballY - playerY;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        int moveStep = 3;
        moveX = (int)((dx / distance) * moveStep);
        moveY = (int)((dy / distance) * moveStep);
    }
    else
    {
        moveX = 0;
        moveY = 0;
    }
}
