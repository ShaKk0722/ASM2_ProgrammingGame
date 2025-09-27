#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <SDL2/SDL.h>
#include "object.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

struct PlayerConfig {
    int width;
    int height;
    int radius;
    int mass;
    float accelerator_factor;
    float maxVel;
};



std::vector<PlayerConfig> loadPlayerConfigs(const std::string& filePath);



class Player : public Object
{
private:
    int delta_position;
    float maxVel_X;
    float maxVel_Y;
    float delta_time;
    bool is_Choose;
    int team;
    SDL_Texture* texture = nullptr;

public:
    Player(int x, int y, int team, const PlayerConfig& config);
    ~Player();
    void update();
    void render();
    bool loadPlayer(const char* texturePath);
    void move(int stateMove, int fieldX, int fieldY, int fieldWidth, int fieldHeight);
};

#endif