#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <SDL2/SDL.h>

class Global
{
public:
    static SDL_Window *window;
    static SDL_Renderer *renderer;
    static SDL_Texture *texture;
    static char *backgroud_path;
    static int HEIGHT;
    static int WIDTH;
    static char *GAME_NAME;
    static int FPS;
    static float g;
    static int delta_time;
    static float friction_k;
    static int team1_color[4];
    static int team2_color[4];
};

#endif