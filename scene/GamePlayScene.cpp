#include "GamePlayScene.h"
#include "../Game.h"
#include <random>
#include <iostream>
#include <cmath>
#include "../math_utils.h"

GamePlayScene::GamePlayScene()
{
    const int groundWidth = 800;
    const int groundHeight = 500;
    const int groundX = (1000 - groundWidth) / 2; // SCREEN_WIDTH - groundWidth
    const int groundY = (800 - groundHeight) / 2; // SCREEN_HEIGHT - groundHeight

    ground = new Ground(groundX, groundY, groundWidth, groundHeight);
    int playerRadius = 20;
    fieldX = ground->getFieldX();
    fieldY = ground->getFieldY();
    fieldWidth = ground->getFieldWidth();
    fieldHeight = ground->getFieldHeight();
    centerX = ground->getCenterX();
    centerY = ground->getCenterY();

    // Load player configurations
    std::vector<PlayerConfig> playerConfigs = loadPlayerConfigs("configs/player_configs.txt"); // Create this file

    // Seed for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, playerConfigs.size() - 1);

    // Create players with random configurations
    team1Players[0] = new Player(fieldX + 100, centerY - 50, 1, playerConfigs[distrib(gen)]);
    team1Players[1] = new Player(fieldX + 100, centerY + 50, 1, playerConfigs[distrib(gen)]);

    team2Players[0] = new Player(fieldX + fieldWidth - 100, centerY - 50, 2, playerConfigs[distrib(gen)]);
    team2Players[1] = new Player(fieldX + fieldWidth - 100, centerY + 50, 2, playerConfigs[distrib(gen)]);

    ball = new Ball(centerX, centerY, 15);
}

GamePlayScene::~GamePlayScene()
{
    delete ground;
    delete team1Players[0];
    delete team1Players[1];
    delete team2Players[0];
    delete team2Players[1];
    delete ball;
}

bool GamePlayScene::loadBackgroundFrames(const std::string &folder, int totalFrames)
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

void GamePlayScene::init(Manager *m)
{
    manager = m;
    ball->loadBall("assets/images/ball.png");
    team1Players[0]->loadPlayer("assets/images/vietnam.png");
    team1Players[1]->loadPlayer("assets/images/vietnam.png");
    team2Players[0]->loadPlayer("assets/images/china.png");
    team2Players[1]->loadPlayer("assets/images/china.png");
    ground->loadGround("assets/images/football_field.jpeg");
    this->loadBackgroundFrames("assets/images/cheering_6", 10); // adjust number of frames
    std::cout << "GamePlayScene initialized!" << std::endl;
}

void GamePlayScene::handleEvents(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN && !event.key.repeat)
    {
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
    float distance_to_ball1 = team1Players[activePlayer1]->get_distance(ball->get_x(),ball->get_y());
    float distance_to_ball2 = team2Players[activePlayer2]->get_distance(ball->get_x(),ball->get_y());
    int state1 = 0, state2 = 0; // 0: idle, 1: attack, 2: defense
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 1);
    if (distance_to_ball1 > distance_to_ball2) {
        state1 = 2;
        state2 = 1;
    }
    else if (distance_to_ball1 < distance_to_ball2) {
        state1 = 1;
        state2 = 2;
    }

    // Team 1 active player: WASD
    if (keyStates[SDL_SCANCODE_W])
    {
        team1Players[activePlayer1]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth, fieldHeight);
    }

    else if (keyStates[SDL_SCANCODE_S])
    {
        team1Players[activePlayer1]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth, fieldHeight);
    }

    else if (keyStates[SDL_SCANCODE_A])
    {
        team1Players[activePlayer1]->move(SDL_SCANCODE_A, fieldX, fieldY, fieldWidth, fieldHeight);
    }

    else if (keyStates[SDL_SCANCODE_D])
    {
        team1Players[activePlayer1]->move(SDL_SCANCODE_D, fieldX, fieldY, fieldWidth, fieldHeight);
    }
    else
    {
        team1Players[activePlayer1]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
    }

    // Team 2 active player: Arrow keys
    if (keyStates[SDL_SCANCODE_UP])
        team2Players[activePlayer2]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth, fieldHeight);
    else if (keyStates[SDL_SCANCODE_DOWN])
        team2Players[activePlayer2]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth, fieldHeight);
    else if (keyStates[SDL_SCANCODE_LEFT])
        team2Players[activePlayer2]->move(SDL_SCANCODE_LEFT, fieldX, fieldY, fieldWidth, fieldHeight);
    else if (keyStates[SDL_SCANCODE_RIGHT])
        team2Players[activePlayer2]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY, fieldWidth, fieldHeight);
    else
        team2Players[activePlayer2]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);

    Uint32 now = SDL_GetTicks();
    if (now - lastNonActiveAIMove >= 5)
    {
        // Team 1 non-active player: AI
        for (int i = 0; i < 2; ++i) {
            if (i!=activePlayer1) {
                int moveY = distrib(gen);
                if(state1==0) team1Players[i]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
                else if (state1 == 1) {
                    team1Players[i]->move(SDL_SCANCODE_D, fieldX, fieldY, fieldWidth, fieldHeight);
                    if (moveY==0) team1Players[i]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth, fieldHeight);
                    else team1Players[i]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth, fieldHeight);
                }
                else if (state1 == 2) {
                    team1Players[i]->move(SDL_SCANCODE_A, fieldX, fieldY, fieldWidth, fieldHeight);
                    if (moveY==0) team1Players[i]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth, fieldHeight);
                    else team1Players[i]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth, fieldHeight);
                }
            }
        }

        // Team 2 non-active player: AI
        for (int i = 0; i < 2; ++i) {
            if (i!=activePlayer2) {
                int moveY = distrib(gen);
                if(state2==0) team2Players[i]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
                else if (state2 == 1) {
                    team2Players[i]->move(SDL_SCANCODE_LEFT, fieldX, fieldY, fieldWidth, fieldHeight);
                    if (moveY==0) team2Players[i]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth, fieldHeight);
                    else team2Players[i]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth, fieldHeight);
                }
                else if (state2 == 2) {
                    team2Players[i]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY, fieldWidth, fieldHeight);
                    if (moveY==0) team2Players[i]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth, fieldHeight);
                    else team2Players[i]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth, fieldHeight);
                }
            }
        }
        lastNonActiveAIMove = now;
    }

    float accelerator_x = 0, accelerator_y = 0;
    for (int i = 0; i < 2; ++i)
    {
        float distance_1 = d2(this->ball->get_x(), this->team1Players[i]->get_x(), this->ball->get_y(), this->team1Players[i]->get_y());
        float distance_2 = d2(this->ball->get_x(), this->team2Players[i]->get_x(), this->ball->get_y(), this->team2Players[i]->get_y());
        if (distance_1 <= this->ball->get_radius() + this->team1Players[i]->get_radius())
        {
            accelerator_x += this->team1Players[i]->get_velocity_x();
            accelerator_y += this->team1Players[i]->get_velocity_y();
        }
        if (distance_2 <= this->ball->get_radius() + this->team2Players[i]->get_radius())
        {
            accelerator_x += this->team2Players[i]->get_velocity_x();
            accelerator_y += this->team2Players[i]->get_velocity_y();
        }
    }
    ball->move(accelerator_x, accelerator_y, fieldX, fieldY, fieldWidth, fieldHeight);

    // Check for goals
    checkGoal();

    now = SDL_GetTicks();
    if (now > lastFrameTime + frameDelay)
    {
        currentFrame = (currentFrame + 1) % backgroundFrames.size();
        lastFrameTime = now;
    }
}

void GamePlayScene::render()
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

    // Draw players
    for (int i = 0; i < 2; ++i)
    {
        // Highlight active player for Team 1
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

        // Highlight active player for Team 2
        if (i == activePlayer2)
        {
            SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 255); // Yellow border
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

    renderScore();
}

void GamePlayScene::checkGoal()
{
    float ballX = ball->get_x();
    float ballY = ball->get_y();
    float ballRadius = ball->get_radius();

    if (ballX - ballRadius <= leftGoal.x + leftGoal.w &&
        ballY >= leftGoal.y && ballY <= leftGoal.y + leftGoal.h)
    {
        if (!goalScored)
        {
            team2Score++;
            goalScored = true;
            goalTeam = 2;
            goalTime = SDL_GetTicks();
            ballResetPending = true;
            std::cout << "GOAL! Team 2 (Blue) scores! Score: " << team1Score << " - " << team2Score << std::endl;
        }
    }

    else if (ballX + ballRadius >= rightGoal.x &&
             ballY >= rightGoal.y && ballY <= rightGoal.y + rightGoal.h)
    {
        if (!goalScored)
        {
            team1Score++;
            goalScored = true;
            goalTeam = 1;
            goalTime = SDL_GetTicks();
            ballResetPending = true;
            std::cout << "GOAL! Team 1 (Red) scores! Score: " << team1Score << " - " << team2Score << std::endl;
        }
    }

    if (ballResetPending && SDL_GetTicks() - goalTime > 1500)
    {
        resetBall();
        ballResetPending = false;
    }

    if (goalScored && SDL_GetTicks() - goalTime > 3000)
    {
        goalScored = false;
        goalTeam = 0;
    }
}

void GamePlayScene::resetBall()
{
    ball->set_x(centerX);
    ball->set_y(centerY);
    ball->set_velocity_x(0);
    ball->set_velocity_y(0);
}

void GamePlayScene::renderScore()
{
    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

    SDL_Rect scoreBox1 = {50, 50, 100, 40};
    SDL_Rect scoreBox2 = {850, 50, 100, 40};
    SDL_RenderFillRect(Game::renderer, &scoreBox1);
    SDL_RenderFillRect(Game::renderer, &scoreBox2);

    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);

    for (int i = 0; i < team1Score && i < 10; i++)
    {
        SDL_Rect dot = {60 + i * 8, 60, 6, 20};
        SDL_RenderFillRect(Game::renderer, &dot);
    }

    for (int i = 0; i < team2Score && i < 10; i++)
    {
        SDL_Rect dot = {860 + i * 8, 60, 6, 20};
        SDL_RenderFillRect(Game::renderer, &dot);
    }

    if (goalScored)
    {
        Uint32 timeSinceGoal = SDL_GetTicks() - goalTime;

        float pulse = (sin(timeSinceGoal * 0.01f) + 1.0f) * 0.5f;
        int alpha = (int)(200 + pulse * 55);

        // Goal message background
        SDL_Rect messageBox = {300, 250, 400, 100};

        if (goalTeam == 1)
        {
            // Red team celebration
            SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, alpha);
        }
        else if (goalTeam == 2)
        {

            SDL_SetRenderDrawColor(Game::renderer, 0, 0, 255, alpha);
        }

        SDL_RenderFillRect(Game::renderer, &messageBox);

        SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(Game::renderer, &messageBox);

        SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

        int textX = 450;
        int textY = 290;

        for (int i = 0; i < 4; i++)
        {
            SDL_Rect letter = {textX + i * 20, textY, 15, 20};
            SDL_RenderFillRect(Game::renderer, &letter);
        }
    }
}
