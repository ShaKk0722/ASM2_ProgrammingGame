#include "GamePlayScene.h"
#include "../Game.h"
#include <random>
#include <iostream>
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


bool GamePlayScene::loadWindFrames(const std::string& folder, int totalFrames) {
    for (int i = 1; i <= totalFrames; i++) {
        char path[256];
        sprintf(path, "%s/frame_%03d.png", folder.c_str(), i);

        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << std::endl;
            continue;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);

        if (tex) {
            windFrames.push_back(tex);
        }
    }

    return !windFrames.empty();
}

bool GamePlayScene::loadBackgroundFrames(const std::string& folder, int totalFrames) {
    for (int i = 1; i <= totalFrames; i++) {
        char path[256];
        sprintf(path, "%s/frame_%03d.png", folder.c_str(), i);

        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << std::endl;
            continue;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);

        if (tex) {
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
    ground->spawnInitialObstacles(2, 40, 120, ball, team1Players, team2Players , 2); // Spawn 2 obstacles in the center 1/3 area
    this->loadBackgroundFrames("assets/images/cheering_6", 10); // adjust number of frames
    this->loadWindFrames("assets/images/wind_2", 5); // adjust number of frames
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
    Uint32 now = SDL_GetTicks();

    // Handle wind effect timing
    if (windActive) {
        // Deactivate wind after its duration
        if (now > windStartTime + windDuration) {
            windActive = false;
            windX = 0; // Reset wind force
            windY = 0;
        }
    } else {
        // Reactivate wind after cooldown
        if (now > windStartTime + windDuration + windCooldown) {
            windActive = true;
            windStartTime = now;

            // Randomize wind direction and strength
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> windDistrib(-0.5f, 0.5f); // Wind strength range (-0.5 to 0.5)

            windX = windDistrib(gen);
            windY = windDistrib(gen);
        }
    }

    // Team 1 active player: WASD
    if (keyStates[SDL_SCANCODE_W]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth, fieldHeight);
    } else if (keyStates[SDL_SCANCODE_S]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth, fieldHeight);
    } else if (keyStates[SDL_SCANCODE_A]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_A, fieldX, fieldY, fieldWidth, fieldHeight);
    } else if (keyStates[SDL_SCANCODE_D]) {
        team1Players[activePlayer1]->move(SDL_SCANCODE_D, fieldX, fieldY, fieldWidth, fieldHeight);
    } else {
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

    float accelerator_x = 0, accelerator_y = 0;
    for (int i = 0; i < 2; ++i) {
        float distance_1 = d2(this->ball->get_x(), this->team1Players[i]->get_x(), this->ball->get_y(), this->team1Players[i]->get_y());
        float distance_2 = d2(this->ball->get_x(), this->team2Players[i]->get_x(), this->ball->get_y(), this->team2Players[i]->get_y());
        if (distance_1 <= this->ball->get_radius() + this->team1Players[i]->get_radius()) {
            accelerator_x += this->team1Players[i]->get_velocity_x();
            accelerator_y += this->team1Players[i]->get_velocity_y();
        }
        if (distance_2 <= this->ball->get_radius() + this->team2Players[i]->get_radius()) {
            accelerator_x += this->team2Players[i]->get_velocity_x();
            accelerator_y += this->team2Players[i]->get_velocity_y();
        }
    }

    // Apply wind effect to the ball if active
    if (windActive) {
        accelerator_x += windX;
        accelerator_y += windY;
    }

    ball->move(accelerator_x, accelerator_y, fieldX, fieldY, fieldWidth, fieldHeight);

    ground->update(now);

    // Update background animation
    if (now > lastFrameTime + frameDelay) {
        currentFrame = (currentFrame + 1) % backgroundFrames.size();
        lastFrameTime = now;
    }
}

void GamePlayScene::render()
{
    if (!backgroundFrames.empty())
    {
        SDL_RenderCopy(Game::renderer, backgroundFrames[currentFrame], NULL, NULL);
    } else {
        std::cout << "No background texture loaded!" << std::endl;
    }
    ground->render();

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

    // Render wind animation on the ground if active
    if (windActive && !windFrames.empty()) {
        // Update wind animation frame
        Uint32 now = SDL_GetTicks();
        if (now > lastWindFrameTime + windFrameDelay) {
            currentWindFrame = (currentWindFrame + 1) % windFrames.size();
            lastWindFrameTime = now;
        }

        // Calculate wind angle based on wind direction
        float angle = atan2(windY, windX) * 180.0f / M_PI; // Convert radians to degrees

        // Render the wind frame with rotation
        SDL_Rect dstRect = {fieldX * 3 / 4, fieldY * 3 / 4, fieldWidth * 3 / 4, fieldHeight * 3 / 4}; // Cover the ground
        SDL_RenderCopyEx(Game::renderer, windFrames[currentWindFrame], NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
    }

}

