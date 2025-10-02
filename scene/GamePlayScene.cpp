#include "GamePlayScene.h"
#include "../Game.h"
#include "../math_utils.h"
#include <cmath>
#include <iostream>
#include <random>

GamePlayScene::GamePlayScene() {
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
  leftGoalTop = {fieldX, fieldY + 150};                  // y = top blue dot
  leftGoalBottom = {fieldX, fieldY + fieldHeight - 150}; // y = bottom blue dot

  rightGoalTop = {fieldX + fieldWidth, fieldY + 150};
  rightGoalBottom = {fieldX + fieldWidth, fieldY + fieldHeight - 150};

  // Load player configurations
  std::vector<PlayerConfig> playerConfigs =
      loadPlayerConfigs("configs/player_configs.txt"); // Create this file

  // Seed for random number generation
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, playerConfigs.size() - 1);

  // Create players with random configurations
  team1Players[0] =
      new Player(fieldX + 100, centerY - 50, 1, playerConfigs[distrib(gen)]);
  team1Players[1] =
      new Player(fieldX + 100, centerY + 50, 1, playerConfigs[distrib(gen)]);

  team2Players[0] = new Player(fieldX + fieldWidth - 100, centerY - 50, 2,
                               playerConfigs[distrib(gen)]);
  team2Players[1] = new Player(fieldX + fieldWidth - 100, centerY + 50, 2,
                               playerConfigs[distrib(gen)]);

  ball = new Ball(centerX, centerY, 15);
}

GamePlayScene::~GamePlayScene() {
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

bool GamePlayScene::loadBackgroundFrames(const std::string &folder,
                                         int totalFrames) {
  for (int i = 1; i <= totalFrames; i++) {
    char path[256];
    sprintf(path, "%s/frame_%03d.png", folder.c_str(), i);

    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
      std::cerr << "Failed to load " << path << ": " << IMG_GetError()
                << std::endl;
      continue;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surface);
    SDL_FreeSurface(surface);

    if (tex) {
      backgroundFrames.push_back(tex);
    }
  }

  return !backgroundFrames.empty();
}

void GamePlayScene::init(Manager *m) {
  manager = m;
  ball->loadBall("assets/images/ball.png");
  team1Players[0]->loadPlayer("assets/images/vietnam.png");
  team1Players[1]->loadPlayer("assets/images/vietnam.png");
  team2Players[0]->loadPlayer("assets/images/thai.png");
  team2Players[1]->loadPlayer("assets/images/thai.png");
  ground->loadGround("assets/images/football_field.jpeg");
  this->loadBackgroundFrames("assets/images/cheering_6",
                             10); // adjust number of frames
                                  // In GamePlayScene::init
  this->loadWindFrames("assets/images/wind_2", 5); // adjust number of frames
  startTime = SDL_GetTicks();

  std::cout << "GamePlayScene initialized!" << std::endl;
}

void GamePlayScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    Game::lastState = Game::gameState;
    Game::gameState = GameState::Pause;
  }

  if (event.type == SDL_MOUSEMOTION) {
    int mx = event.motion.x, my = event.motion.y;
    pauseHovered =
        (mx >= pauseButton.x && mx <= pauseButton.x + pauseButton.w &&
         my >= pauseButton.y && my <= pauseButton.y + pauseButton.h);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    if (pauseHovered) {
      Game::lastState = Game::gameState;
      Game::gameState = GameState::Pause;
    }
  }

  if (event.type == SDL_KEYDOWN && !event.key.repeat) {
    keyStates[event.key.keysym.scancode] = true;
    // Team 1: TAB to switch player
    if (event.key.keysym.sym == SDLK_TAB)
      activePlayer1 = 1 - activePlayer1;

    // Team 2: ENTER to switch player
    if (event.key.keysym.sym == SDLK_RETURN)
      activePlayer2 = 1 - activePlayer2;
  } else if (event.type == SDL_KEYUP)
    keyStates[event.key.keysym.scancode] = false;
  // Update hovers if match over
  if (matchOver && event.type == SDL_MOUSEMOTION) {
    int mx = event.motion.x, my = event.motion.y;
    againHov = (mx >= againBtn.x && mx <= againBtn.x + againBtn.w &&
                my >= againBtn.y && my <= againBtn.y + againBtn.h);
    backHov = (mx >= backBtn.x && mx <= backBtn.x + backBtn.w &&
               my >= backBtn.y && my <= backBtn.y + backBtn.h);
  }

  if (matchOver && event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    if (againHov) {
      // Reset match but keep same scene
      team1Score = team2Score = 0;
      matchOver = false;
      winnerTeam = 0;
      resetBall();
      startTime = SDL_GetTicks();
      return;
    }
    if (backHov) {
      Game::switchToMainMenu(); // or Game::gameState = GameState::MainMenu;
      return;
    }
  }
}

void GamePlayScene::update() {
  Uint32 now = SDL_GetTicks();

  float distance_to_ball1 =
      team1Players[activePlayer1]->get_distance(ball->get_x(), ball->get_y());
  float distance_to_ball2 =
      team2Players[activePlayer2]->get_distance(ball->get_x(), ball->get_y());
  int state1 = 0, state2 = 0; // 0: idle, 1: attack, 2: defense
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 1);
  if (distance_to_ball1 > distance_to_ball2) {
    state1 = 2;
    state2 = 1;
  } else if (distance_to_ball1 < distance_to_ball2) {
    state1 = 1;
    state2 = 2;
  }
  if (windActive) {
      // Deactivate wind after its duration
      if (now > windStartTime + windDuration) {
          windActive = false;
          windX = 0; // Reset wind force
          windY = 0;
      }
  } 
  else {
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
  if (keyStates[SDL_SCANCODE_W])
    team1Players[activePlayer1]->move(SDL_SCANCODE_W, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_S])
    team1Players[activePlayer1]->move(SDL_SCANCODE_S, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_A])
    team1Players[activePlayer1]->move(SDL_SCANCODE_A, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_D])
    team1Players[activePlayer1]->move(SDL_SCANCODE_D, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else
    team1Players[activePlayer1]->move(-1, fieldX, fieldY, fieldWidth,
                                      fieldHeight);
  // Team 2 active player: Arrow keys
  if (keyStates[SDL_SCANCODE_UP])
    team2Players[activePlayer2]->move(SDL_SCANCODE_UP, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_DOWN])
    team2Players[activePlayer2]->move(SDL_SCANCODE_DOWN, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_LEFT])
    team2Players[activePlayer2]->move(SDL_SCANCODE_LEFT, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else if (keyStates[SDL_SCANCODE_RIGHT])
    team2Players[activePlayer2]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  else
    team2Players[activePlayer2]->move(-1, fieldX, fieldY, fieldWidth,
                                      fieldHeight);
  if (now - lastNonActiveAIMove >= 5) {
    // Team 1 non-active player: AI
    for (int i = 0; i < 2; ++i) {
      if (i != activePlayer1) {
        int moveY = distrib(gen);
        if (state1 == 0)
          team1Players[i]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
        else if (state1 == 1) {
          team1Players[i]->move(SDL_SCANCODE_D, fieldX, fieldY, fieldWidth,
                                fieldHeight);
          if (moveY == 0)
            team1Players[i]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
          else
            team1Players[i]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
        } else if (state1 == 2) {
          team1Players[i]->move(SDL_SCANCODE_A, fieldX, fieldY, fieldWidth,
                                fieldHeight);
          if (moveY == 0)
            team1Players[i]->move(SDL_SCANCODE_W, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
          else
            team1Players[i]->move(SDL_SCANCODE_S, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
        }
      }
    }

    // Team 2 non-active player: AI
    for (int i = 0; i < 2; ++i) {
      if (i != activePlayer2) {
        int moveY = distrib(gen);
        if (state2 == 0)
          team2Players[i]->move(-1, fieldX, fieldY, fieldWidth, fieldHeight);
        else if (state2 == 1) {
          team2Players[i]->move(SDL_SCANCODE_LEFT, fieldX, fieldY, fieldWidth,
                                fieldHeight);
          if (moveY == 0)
            team2Players[i]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
          else
            team2Players[i]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
        } else if (state2 == 2) {
          team2Players[i]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY, fieldWidth,
                                fieldHeight);
          if (moveY == 0)
            team2Players[i]->move(SDL_SCANCODE_UP, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
          else
            team2Players[i]->move(SDL_SCANCODE_DOWN, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
        }
      }
    }
    lastNonActiveAIMove = now;
  }
                                      
  float accelerator_x = 0, accelerator_y = 0;

  for (int i = 0; i < 2; ++i) {
    float distance_1 = d2(ball->get_x(), team1Players[i]->get_x(),
                          ball->get_y(), team1Players[i]->get_y());
    float distance_2 = d2(ball->get_x(), team2Players[i]->get_x(),
                          ball->get_y(), team2Players[i]->get_y());

    if (distance_1 <= ball->get_radius() + team1Players[i]->get_radius()) {
      accelerator_x += team1Players[i]->get_velocity_x();
      accelerator_y += team1Players[i]->get_velocity_y();

      if (Game::sfxEnabled && Game::kickSound)
        Mix_PlayChannel(-1, Game::kickSound, 0);
    }

    if (distance_2 <= ball->get_radius() + team2Players[i]->get_radius()) {
      accelerator_x += team2Players[i]->get_velocity_x();
      accelerator_y += team2Players[i]->get_velocity_y();

      if (Game::sfxEnabled && Game::kickSound)
        Mix_PlayChannel(-1, Game::kickSound, 0);
    }
  }
  // Apply wind effect to the ball if active
  if (windActive) {
      accelerator_x += windX;
      accelerator_y += windY;
  }

  ball->move(accelerator_x, accelerator_y, fieldX, fieldY, fieldWidth,
             fieldHeight);

  // Check for goals
  checkGoal();

  
  elapsedSeconds = (now - startTime) / 1000;

  if (now > lastFrameTime + frameDelay) {
    currentFrame = (currentFrame + 1) % backgroundFrames.size();
    lastFrameTime = now;
  }


  if (matchOver)
    return;
  if (now > lastFrameTime + frameDelay) {
    currentFrame = (currentFrame + 1) % backgroundFrames.size();
    lastFrameTime = now;
  }
}

void GamePlayScene::render() {
  if (!backgroundFrames.empty()) {
    SDL_RenderCopy(Game::renderer, backgroundFrames[currentFrame], NULL, NULL);
  } else {
    std::cout << "No background texture loaded!" << std::endl;
  }
  ground->render();
  // drawpause
  //  In GamePlayScene.cpp render()
  SDL_SetRenderDrawColor(Game::renderer, pauseHovered ? 200 : 100, 100, 200,
                         255);
  SDL_RenderFillRect(Game::renderer, &pauseButton);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface *surf =
      TTF_RenderText_Blended(Game::settingsScene.getFont(), "Pause", white);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, surf);
  SDL_Rect txt = {pauseButton.x + pauseButton.w / 2 - surf->w / 2,
                  pauseButton.y + pauseButton.h / 2 - surf->h / 2, surf->w,
                  surf->h};
  SDL_FreeSurface(surf);
  SDL_RenderCopy(Game::renderer, tex, nullptr, &txt);
  SDL_DestroyTexture(tex);

  // draw time
  //  In GamePlayScene::render (draw timer at top-center)
  char timerBuf[32];
  sprintf(timerBuf, "Time: %d", elapsedSeconds);
  SDL_Surface *timerSurf =
      TTF_RenderText_Blended(Game::settingsScene.getFont(), timerBuf, white);
  SDL_Texture *timerTex =
      SDL_CreateTextureFromSurface(Game::renderer, timerSurf);
  SDL_Rect timerRect = {500 - timerSurf->w / 2, 80, timerSurf->w, timerSurf->h};
  SDL_RenderCopy(Game::renderer, timerTex, nullptr, &timerRect);
  SDL_FreeSurface(timerSurf);
  SDL_DestroyTexture(timerTex);

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
          int distSq = dx * dx + dy * dy;
          if (distSq <= borderRadius * borderRadius &&
              distSq >= (borderRadius - 2) * (borderRadius - 2)) {
            SDL_RenderDrawPoint(Game::renderer, team1Players[i]->get_x() + dx,
                                team1Players[i]->get_y() + dy);
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
          int distSq = dx * dx + dy * dy;
          if (distSq <= borderRadius * borderRadius &&
              distSq >= (borderRadius - 2) * (borderRadius - 2)) {
            SDL_RenderDrawPoint(Game::renderer, team2Players[i]->get_x() + dx,
                                team2Players[i]->get_y() + dy);
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

  renderScore();
  if (matchOver) {
    // Dim overlay
    SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 1000, 800};
    SDL_RenderFillRect(Game::renderer, &overlay);

    // Title: which team wins
    SDL_Color white = {255, 255, 255, 255};
    char title[64];
    sprintf(title, "Team %d Wins!", winnerTeam);
    SDL_Surface *tSurf =
        TTF_RenderText_Blended(Game::settingsScene.getFont(), title, white);
    SDL_Texture *tTex = SDL_CreateTextureFromSurface(Game::renderer, tSurf);
    SDL_Rect tRect = {500 - tSurf->w / 2, 240, tSurf->w, tSurf->h};
    SDL_FreeSurface(tSurf);
    SDL_RenderCopy(Game::renderer, tTex, nullptr, &tRect);
    SDL_DestroyTexture(tTex);

    // Score
    char scoreBuf[64];
    sprintf(scoreBuf, "Final Score: %d - %d", team1Score, team2Score);
    SDL_Surface *sSurf =
        TTF_RenderText_Blended(Game::settingsScene.getFont(), scoreBuf, white);
    SDL_Texture *sTex = SDL_CreateTextureFromSurface(Game::renderer, sSurf);
    SDL_Rect sRect = {500 - sSurf->w / 2, 285, sSurf->w, sSurf->h};
    SDL_FreeSurface(sSurf);
    SDL_RenderCopy(Game::renderer, sTex, nullptr, &sRect);
    SDL_DestroyTexture(sTex);

    // Time
    char timeBuf[64];
    sprintf(timeBuf, "Time: %d sec", elapsedSeconds);
    SDL_Surface *tmSurf =
        TTF_RenderText_Blended(Game::settingsScene.getFont(), timeBuf, white);
    SDL_Texture *tmTex = SDL_CreateTextureFromSurface(Game::renderer, tmSurf);
    SDL_Rect tmRect = {500 - tmSurf->w / 2, 315, tmSurf->w, tmSurf->h};
    SDL_FreeSurface(tmSurf);
    SDL_RenderCopy(Game::renderer, tmTex, nullptr, &tmRect);
    SDL_DestroyTexture(tmTex);

    // Buttons: Play Again / Back
    auto drawBtn = [&](SDL_Rect r, const char *label, bool hov) {
      SDL_SetRenderDrawColor(Game::renderer, hov ? 200 : 100, 100, 200, 255);
      SDL_RenderFillRect(Game::renderer, &r);
      SDL_Surface *bSurf =
          TTF_RenderText_Blended(Game::settingsScene.getFont(), label, white);
      SDL_Texture *bTex = SDL_CreateTextureFromSurface(Game::renderer, bSurf);
      SDL_Rect bRect = {r.x + r.w / 2 - bSurf->w / 2,
                        r.y + r.h / 2 - bSurf->h / 2, bSurf->w, bSurf->h};
      SDL_FreeSurface(bSurf);
      SDL_RenderCopy(Game::renderer, bTex, nullptr, &bRect);
      SDL_DestroyTexture(bTex);
    };
    drawBtn(againBtn, "Play Again", againHov);
    drawBtn(backBtn, "Back to Menu", backHov);

    SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_NONE);
  }
}

void GamePlayScene::checkGoal() {
  float ballX = ball->get_x();
  float ballY = ball->get_y();
  float ballRadius = ball->get_radius();

  // Left goal check
  if (ballX - ballRadius <= leftGoalTop.x && ballY >= leftGoalTop.y &&
      ballY <= leftGoalBottom.y) {
    if (!goalScored) {
      team2Score++;
      goalScored = true;
      goalTeam = 2;
      goalTime = SDL_GetTicks();
      ballResetPending = true;
      std::cout << "GOAL! Team 2 scores! " << team1Score << " - " << team2Score
                << std::endl;
    }
  }

  else if (ballX + ballRadius >= rightGoalTop.x && ballY >= rightGoalTop.y &&
           ballY <= rightGoalBottom.y) {
    if (!goalScored) {
      team1Score++;
      goalScored = true;
      goalTeam = 1;
      goalTime = SDL_GetTicks();
      ballResetPending = true;
      std::cout << "GOAL! Team 1 scores! " << team1Score << " - " << team2Score
                << std::endl;
    }
  }

  if (ballResetPending && SDL_GetTicks() - goalTime > 1500) {
    resetBall();
    ballResetPending = false;
  }

  if (goalScored && SDL_GetTicks() - goalTime > 3000) {
    goalScored = false;
    goalTeam = 0;
  }
  if (team1Score == 3 || team2Score == 3) {
    matchOver = true;
    winnerTeam = (team1Score == 3) ? 1 : 2;
  }
}

void GamePlayScene::resetBall() {
  ball->set_x(centerX);
  ball->set_y(centerY);
  ball->set_velocity_x(0);
  ball->set_velocity_y(0);
  team1Players[0]->setPosition(fieldX + 100, centerY - 50);
  team1Players[1]->setPosition(fieldX + 100, centerY + 50);
  team2Players[0]->setPosition(fieldX + fieldWidth - 100, centerY - 50);
  team2Players[1]->setPosition(fieldX + fieldWidth - 100, centerY + 50);
}

void GamePlayScene::renderScore() {
  SDL_Color white = {255, 255, 255, 255};
  SDL_Rect scoreBar = {350, 20, 300, 60};
  SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 180);
  SDL_RenderFillRect(Game::renderer, &scoreBar);
  SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(Game::renderer, &scoreBar);

  char buf[16];
  sprintf(buf, "%d", team1Score);
  SDL_Surface *surf1 =
      TTF_RenderText_Blended(Game::settingsScene.getFont(), buf, white);
  SDL_Texture *tex1 = SDL_CreateTextureFromSurface(Game::renderer, surf1);
  SDL_Rect r1 = {scoreBar.x + 60 - surf1->w / 2, scoreBar.y + 30 - surf1->h / 2,
                 surf1->w, surf1->h};
  SDL_RenderCopy(Game::renderer, tex1, nullptr, &r1);
  SDL_FreeSurface(surf1);
  SDL_DestroyTexture(tex1);

  SDL_Surface *sep =
      TTF_RenderText_Blended(Game::settingsScene.getFont(), "-", white);
  SDL_Texture *sepTex = SDL_CreateTextureFromSurface(Game::renderer, sep);
  SDL_Rect rSep = {scoreBar.x + scoreBar.w / 2 - sep->w / 2,
                   scoreBar.y + 30 - sep->h / 2, sep->w, sep->h};
  SDL_RenderCopy(Game::renderer, sepTex, nullptr, &rSep);
  SDL_FreeSurface(sep);
  SDL_DestroyTexture(sepTex);

  sprintf(buf, "%d", team2Score);
  SDL_Surface *surf2 =
      TTF_RenderText_Blended(Game::settingsScene.getFont(), buf, white);
  SDL_Texture *tex2 = SDL_CreateTextureFromSurface(Game::renderer, surf2);
  SDL_Rect r2 = {scoreBar.x + scoreBar.w - 60 - surf2->w / 2,
                 scoreBar.y + 30 - surf2->h / 2, surf2->w, surf2->h};
  SDL_RenderCopy(Game::renderer, tex2, nullptr, &r2);
  SDL_FreeSurface(surf2);
  SDL_DestroyTexture(tex2);
}
void GamePlayScene::reset() {
  team1Score = 0;
  team2Score = 0;
  goalScored = false;
  goalTeam = 0;
  ballResetPending = false;
  matchOver = false;
  winnerTeam = 0;
  startTime = SDL_GetTicks();
  elapsedSeconds = 0;

  // Reset ball to center
  resetBall();

  // Reset players to initial positions
  team1Players[0]->setPosition(fieldX + 100, centerY - 50);
  team1Players[1]->setPosition(fieldX + 100, centerY + 50);
  team2Players[0]->setPosition(fieldX + fieldWidth - 100, centerY - 50);
  team2Players[1]->setPosition(fieldX + fieldWidth - 100, centerY + 50);
}
