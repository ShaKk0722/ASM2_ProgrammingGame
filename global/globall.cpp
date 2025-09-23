#include "global.h"

// Initialize static SDL components
SDL_Window *Global::window = nullptr;
SDL_Renderer *Global::renderer = nullptr;
SDL_Texture *Global::texture = nullptr;

// Game window configuration
char *Global::backgroud_path = (char *)"assets/images/background_menu.png";
int Global::HEIGHT = 600;
int Global::WIDTH = 1000;
char *Global::GAME_NAME = (char *)"2D SOCCER";
int Global::FPS = 60;

// Physics constants for 2D Soccer-style gameplay
float Global::g = 9.81f;                     // Gravity (top-down view, no gravity)
int Global::delta_time = 1000 / Global::FPS; // Delta time for 60 FPS
float Global::friction_k = 0.98f;            // Friction coefficient for ball/player movement

// Team colors (RGBA format)
int Global::team1_color[4] = {255, 0, 0, 255}; // Red team
int Global::team2_color[4] = {0, 0, 255, 255}; // Blue team
