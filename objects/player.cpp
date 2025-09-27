
#include "player.h"
#include "../Game.h"
#include <algorithm>
#include <cmath>

std::vector<PlayerConfig> loadPlayerConfigs(const std::string& filePath) {
    std::vector<PlayerConfig> configs;
    std::ifstream file(filePath);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            PlayerConfig config;
            std::string value;

            if (std::getline(ss, value, ',')) config.width = std::stoi(value);
            if (std::getline(ss, value, ',')) config.height = std::stoi(value);
            if (std::getline(ss, value, ',')) config.radius = std::stoi(value);
            if (std::getline(ss, value, ',')) config.mass = std::stoi(value);
            if (std::getline(ss, value, ',')) config.accelerator_factor = std::stof(value);
            if (std::getline(ss, value, ',')) config.maxVel = std::stof(value);

            configs.push_back(config);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
    }

    return configs;
}

void Player::update()
{
    return;
}

void Player::render()
{
    if (texture) {
        SDL_Rect dst;
        dst.x = x - radius;
        dst.y = y - radius;
        dst.w = radius * 2;
        dst.h = radius * 2;
        SDL_RenderCopy(Game::renderer, texture, nullptr, &dst);
    } else {
        SDL_Color color = (team == 1) ? SDL_Color{0, 0, 255, 255} : SDL_Color{255, 0, 0, 255};
        SDL_SetRenderDrawColor(Game::renderer, color.r, color.g, color.b, color.a);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(Game::renderer, x + dx, y + dy);
                }
            }
        }
    }
}

Player::Player(int x, int y, int team, const PlayerConfig& config) : Object()
{
    this->x = x;
    this->y = y;
    this->width = config.width;
    this->height = config.height;
    this->radius = config.radius;
    this->team = team;
    this->acceleration_x = config.accelerator_factor;
    this->acceleration_y = config.accelerator_factor;
    this->mass = config.mass;
    this->velocity_x = 0;
    this->velocity_y = 0;
    this->maxVel_X = config.maxVel;
    this->maxVel_Y = config.maxVel;
}

Player::~Player()
{
    return;
}

void Player::move(int stateMove, int fieldX, int fieldY, int fieldWidth, int fieldHeight)
{
    if (stateMove == SDL_SCANCODE_S || stateMove == SDL_SCANCODE_DOWN) {
        this->velocity_y += this->acceleration_y;
        // Clamp to maxVel_Y
        if (this->velocity_y > this->maxVel_Y) {
            this->velocity_y = this->maxVel_Y;
        }
    }
        
    else if (stateMove == SDL_SCANCODE_W || stateMove == SDL_SCANCODE_UP) {
        // Increase velocity in the negative Y direction (up)
        this->velocity_y -= this->acceleration_y;
        // Clamp to -maxVel_Y
        if (this->velocity_y < -this->maxVel_Y) {
            this->velocity_y = -this->maxVel_Y;
        }
    }

    else if (stateMove == SDL_SCANCODE_D || stateMove == SDL_SCANCODE_RIGHT) {
        // Increase velocity in the positive X direction (right)
        this->velocity_x += this->acceleration_x;
        // Clamp to maxVel_X
        if (this->velocity_x > this->maxVel_X) {
            this->velocity_x = this->maxVel_X;
        }
    }
        
    else if (stateMove == SDL_SCANCODE_A || stateMove == SDL_SCANCODE_LEFT) {
        // Increase velocity in the negative X direction (left)
        this->velocity_x -= this->acceleration_x;
        // Clamp to -maxVel_X
        if (this->velocity_x < -this->maxVel_X) {
            this->velocity_x = -this->maxVel_X;
        }
    }
    else {
        this->velocity_x *= 0.60f; // Reduce X velocity by 40%
        this->velocity_y *= 0.60f; // Reduce Y velocity by 40%
        
        // Stop completely if velocity is very close to zero to prevent tiny, perpetual movement
        const float STOP_THRESHOLD = 0.01f;
        if (std::abs(this->velocity_x) < STOP_THRESHOLD) {
            this->velocity_x = 0.0f;
        }
        if (std::abs(this->velocity_y) < STOP_THRESHOLD) {
            this->velocity_y = 0.0f;
        }
    }

    float newX = x + this->velocity_x;
    float newY = y + this->velocity_y;

    // Clamp to field boundaries
    int minX = fieldX + radius;
    int maxX = fieldX + fieldWidth - radius;
    int minY = fieldY + radius;
    int maxY = fieldY + fieldHeight - radius;

    if (newX < minX) {
        newX = minX;
        this->velocity_x = 0.0f; // Stop movement against the boundary
    }
    if (newX > maxX) {
        newX = maxX;
        this->velocity_x = 0.0f; // Stop movement against the boundary
    }
    if (newY < minY) {
        newY = minY;
        this->velocity_y = 0.0f; // Stop movement against the boundary
    }
    if (newY > maxY) {
        newY = maxY;
        this->velocity_y = 0.0f; // Stop movement against the boundary
    }

    x = newX;
    y = newY;
}


bool Player::loadPlayer(const char* texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
        SDL_FreeSurface(surface);
        if (texture) {
            std::cout << "Player image loaded successfully!" << std::endl;
            return true;
        } else {
            std::cout << "Failed to create player texture: " << SDL_GetError() << std::endl;
            return false;
        }
    } else {
        std::cout << "Failed to load player image: " << IMG_GetError() << std::endl;
        return false;
    }
}