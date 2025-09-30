#include "GamePlayWithAIScene.h"
#include "../Game.h"
#include <iostream>
#include <random>
#include "../math_utils.h"

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
    // Team 1 active player: WASD
    if (keyStates[SDL_SCANCODE_W]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth, fieldHeight);
    }
        
    else if (keyStates[SDL_SCANCODE_S]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth, fieldHeight);
    }
        
    else if (keyStates[SDL_SCANCODE_A]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_A, fieldX, fieldY, fieldWidth, fieldHeight);
    }
        
    else if (keyStates[SDL_SCANCODE_D]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_D, fieldX, fieldY, fieldWidth, fieldHeight);
    }
    else {
        team1Players[activePlayer1]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
    }

    updateAI();

    float accelerator_x = 0, accelerator_y = 0;
    for(int i = 0; i < 2; ++i) {
        float distance_1 = d2(this->ball->get_x(),this->team1Players[i]->get_x(),this->ball->get_y(), this->team1Players[i]->get_y());
        float distance_2 = d2(this->ball->get_x(),this->team2Players[i]->get_x(),this->ball->get_y(), this->team2Players[i]->get_y());
        if(distance_1 <= this->ball->get_radius() + this->team1Players[i]->get_radius()) {
            accelerator_x += this->team1Players[i]->get_velocity_x();
            accelerator_y += this->team1Players[i]->get_velocity_y();
        }
        if (distance_2 <= this->ball->get_radius() + this->team2Players[i]->get_radius()) {
            accelerator_x += this->team2Players[i]->get_velocity_x();
            accelerator_y += this->team2Players[i]->get_velocity_y();
        }
    }
    ball->move(accelerator_x,accelerator_y,fieldX, fieldY, fieldWidth, fieldHeight);

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

    if (now - lastAIMove >= 10)
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
    float dx = ball->get_x() - team2Players[playerIndex]->get_x();
    float dy = ball->get_y() - team2Players[playerIndex]->get_y();
    float threshold = ball->get_radius() + team2Players[playerIndex]->get_radius();
    float dis = std::pow(std::pow(dx,2) + std::pow(dy,2),0.5);

    if (dis > threshold) {
        if (dx > 0) {
            team2Players[playerIndex]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY, fieldWidth, fieldHeight);
        }
        else if (dx < 0) {
            team2Players[playerIndex]->move(SDL_SCANCODE_LEFT, fieldX, fieldY, fieldWidth, fieldHeight);
        }
        if (dy > 0) {
            team2Players[playerIndex]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth, fieldHeight);
        }
        else if (dy < 0) {
            team2Players[playerIndex]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth, fieldHeight);
        }
    }
    else team2Players[playerIndex]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
}

int GamePlayWithAIScene::findBestAIPlayer()
{

    float ballX = ball->get_x();
    float ballY = ball->get_y();

    float dist0 = sqrt(pow(team2Players[0]->get_x() - ballX, 2) + pow(team2Players[0]->get_y() - ballY, 2));
    float dist1 = sqrt(pow(team2Players[1]->get_x() - ballX, 2) + pow(team2Players[1]->get_y() - ballY, 2));

    return (dist0 < dist1) ? 0 : 1;
}