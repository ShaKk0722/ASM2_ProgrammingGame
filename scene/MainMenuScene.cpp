#include "MainMenuScene.h"
#include "../Game.h"
#include <iostream>

MainMenuScene::MainMenuScene()
{
    backgroundTexture = nullptr;
    isPlayWithPlayerHovered = false;
    isPlayWithAIHovered = false;

    // Button positions (centered on screen)
    int buttonWidth = 300;
    int buttonHeight = 80;
    int screenWidth = 1000;
    int screenHeight = 600;

    playWithPlayerButton = {
        (screenWidth - buttonWidth) / 2,
        screenHeight / 2 - 60,
        buttonWidth,
        buttonHeight};

    playWithAIButton = {
        (screenWidth - buttonWidth) / 2,
        screenHeight / 2 + 40,
        buttonWidth,
        buttonHeight};
}

MainMenuScene::~MainMenuScene()
{
    if (backgroundTexture)
    {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void MainMenuScene::init(Manager *m)
{
    manager = m;
    loadBackground();
    std::cout << "MainMenuScene initialized!" << std::endl;
}

void MainMenuScene::loadBackground()
{
    // For now, we'll use a colored background instead of loading image
    // TODO: Add SDL_image library later for texture loading
    backgroundTexture = nullptr;
    std::cout << "Using fallback stadium-style background" << std::endl;
}

void MainMenuScene::handleEvents(SDL_Event event)
{
    if (event.type == SDL_MOUSEMOTION)
    {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;

        isPlayWithPlayerHovered = isPointInRect(mouseX, mouseY, playWithPlayerButton);
        isPlayWithAIHovered = isPointInRect(mouseX, mouseY, playWithAIButton);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        if (isPointInRect(mouseX, mouseY, playWithPlayerButton))
        {
            std::cout << "Play With Player clicked!" << std::endl;
            Game::switchToGamePlay();
        }

        if (isPointInRect(mouseX, mouseY, playWithAIButton))
        {
            std::cout << "Play With AI clicked!" << std::endl;
            Game::switchToGamePlayWithAI();
        }
    }
}

void MainMenuScene::update()
{
    // Update any animations or menu logic here
}

void MainMenuScene::render()
{
    // Create a stadium-style gradient background
    SDL_SetRenderDrawColor(Game::renderer, 34, 139, 34, 255); // Soccer field green
    SDL_RenderClear(Game::renderer);

    // Draw stadium stands (darker areas)
    SDL_SetRenderDrawColor(Game::renderer, 100, 100, 100, 255); // Gray stands
    SDL_Rect topStand = {0, 0, 1000, 100};
    SDL_Rect bottomStand = {0, 500, 1000, 100};
    SDL_RenderFillRect(Game::renderer, &topStand);
    SDL_RenderFillRect(Game::renderer, &bottomStand);

    // Draw field lines
    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255); // White lines
    SDL_Rect centerLine = {495, 100, 10, 400};
    SDL_RenderFillRect(Game::renderer, &centerLine);

    // Draw "Play With Player" button
    if (isPlayWithPlayerHovered)
    {
        SDL_SetRenderDrawColor(Game::renderer, 100, 200, 100, 255); // Light green when hovered
    }
    else
    {
        SDL_SetRenderDrawColor(Game::renderer, 50, 150, 50, 255); // Dark green
    }
    SDL_RenderFillRect(Game::renderer, &playWithPlayerButton);

    // Button border
    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(Game::renderer, &playWithPlayerButton);

    // Draw "Play With AI" button
    if (isPlayWithAIHovered)
    {
        SDL_SetRenderDrawColor(Game::renderer, 100, 100, 200, 255); // Light blue when hovered
    }
    else
    {
        SDL_SetRenderDrawColor(Game::renderer, 50, 50, 150, 255); // Dark blue
    }
    SDL_RenderFillRect(Game::renderer, &playWithAIButton);

    // Button border
    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(Game::renderer, &playWithAIButton);

    // TODO: Add text rendering for button labels
    // For now, the colors indicate which button is which:
    // Green = Play With Player
    // Blue = Play With AI
}

bool MainMenuScene::isPointInRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}
