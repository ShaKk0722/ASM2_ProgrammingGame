#include "GamePlayWithAIScene.h"
#include "../Game.h"
#include "../math_utils.h"
#include <iostream>
#include <random>

GamePlayWithAIScene::GamePlayWithAIScene() {
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

GamePlayWithAIScene::~GamePlayWithAIScene() {
  delete ground;
  delete team1Players[0];
  delete team1Players[1];
  delete team2Players[0];
  delete team2Players[1];
  delete ball;
}

bool GamePlayWithAIScene::loadBackgroundFrames(const std::string &folder,
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

void GamePlayWithAIScene::init(Manager *m) {
  manager = m;
  ball->loadBall("assets/images/ball.png");
  team1Players[0]->loadPlayer("assets/images/vietnam.png");
  team1Players[1]->loadPlayer("assets/images/vietnam.png");
  team2Players[0]->loadPlayer("assets/images/china.png");
  team2Players[1]->loadPlayer("assets/images/china.png");
  ground->loadGround("assets/images/football_field.jpeg");
  this->loadBackgroundFrames("assets/images/cheering_6", 10);
  // In GamePlayScene::init
  startTime = SDL_GetTicks();

  std::cout << "GamePlayWithAIScene initialized!" << std::endl;
}

void GamePlayWithAIScene::handleEvents(SDL_Event event) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    Game::lastState = Game::gameState;
    Game::gameState = GameState::Pause;
  }
  // In GamePlayScene.cpp handleEvents
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
    if (event.key.keysym.sym == SDLK_TAB)
      activePlayer1 = 1 - activePlayer1;
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

void GamePlayWithAIScene::update() {
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

  updateAI();

  float accelerator_x = 0, accelerator_y = 0;

  for (int i = 0; i < 2; ++i) {
    float distance_1 = d2(ball->get_x(), team1Players[i]->get_x(),
                          ball->get_y(), team1Players[i]->get_y());
    float distance_2 = d2(ball->get_x(), team2Players[i]->get_x(),
                          ball->get_y(), team2Players[i]->get_y());

    // Collision: Team 1
    if (distance_1 <= ball->get_radius() + team1Players[i]->get_radius()) {
      accelerator_x += team1Players[i]->get_velocity_x();
      accelerator_y += team1Players[i]->get_velocity_y();

      if (Game::sfxEnabled && Game::kickSound)
        Mix_PlayChannel(-1, Game::kickSound, 0);
    }

    // Collision: Team 2 (AI)
    if (distance_2 <= ball->get_radius() + team2Players[i]->get_radius()) {
      accelerator_x += team2Players[i]->get_velocity_x();
      accelerator_y += team2Players[i]->get_velocity_y();

      if (Game::sfxEnabled && Game::kickSound)
        Mix_PlayChannel(-1, Game::kickSound, 0);
    }
  }

  // Move ball with accumulated momentum
  ball->move(accelerator_x, accelerator_y, fieldX, fieldY, fieldWidth,
             fieldHeight);

  // Check for goals & reset if needed
  checkGoal();

  // Animate background
  Uint32 now = SDL_GetTicks();
  if (now > lastFrameTime + frameDelay) {
    currentFrame = (currentFrame + 1) % backgroundFrames.size();
    lastFrameTime = now;
  }
  // In GamePlayScene::update
  elapsedSeconds = (now - startTime) / 1000;
  if (matchOver)
    return;
}

void GamePlayWithAIScene::render() {
  if (!backgroundFrames.empty()) {
    SDL_RenderCopy(Game::renderer, backgroundFrames[currentFrame], NULL, NULL);
  } else {
    std::cout << "No background texture loaded!" << std::endl;
  }
  ground->render();

  for (int i = 0; i < 2; ++i) {
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
  }

  for (int i = 0; i < 2; ++i) {
    int bestAIPlayer = findBestAIPlayer();
    if (i == bestAIPlayer) {
      SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0,
                             255); // Red border for AI
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
  // draw pause
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
  // draw time // In GamePlayScene::render (draw timer at top-center)
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

void GamePlayWithAIScene::updateAI() {
  Uint32 now = SDL_GetTicks();

  if (now - lastAIMove >= 300) {
    int bestAIPlayer = findBestAIPlayer();
    moveAIPlayer(bestAIPlayer);
    lastAIMove = now;
  }
}

void GamePlayWithAIScene::moveAIPlayer(int playerIndex) {
  if (playerIndex < 0 || playerIndex >= 2)
    return;

  int moveX = 0, moveY = 0;
  calculateAIMove(playerIndex, moveX, moveY);

  team2Players[playerIndex]->move(moveY, fieldX, fieldY, fieldWidth,
                                  fieldHeight);
}

int GamePlayWithAIScene::findBestAIPlayer() {

  float ballX = ball->get_x();
  float ballY = ball->get_y();

  float dist0 = sqrt(pow(team2Players[0]->get_x() - ballX, 2) +
                     pow(team2Players[0]->get_y() - ballY, 2));
  float dist1 = sqrt(pow(team2Players[1]->get_x() - ballX, 2) +
                     pow(team2Players[1]->get_y() - ballY, 2));

  return (dist0 < dist1) ? 0 : 1;
}

void GamePlayWithAIScene::calculateAIMove(int playerIndex, int &moveX,
                                          int &moveY) {

  float ballX = ball->get_x();
  float ballY = ball->get_y();
  float playerX = team2Players[playerIndex]->get_x();
  float playerY = team2Players[playerIndex]->get_y();

  float dx = ballX - playerX;
  float dy = ballY - playerY;
  float distance = sqrt(dx * dx + dy * dy);

  if (distance > 0) {
    int moveStep = 3;
    moveX = (int)((dx / distance) * moveStep);
    moveY = (int)((dy / distance) * moveStep);
  } else {
    moveX = 0;
    moveY = 0;
  }
}

void GamePlayWithAIScene::renderScore() {
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

void GamePlayWithAIScene::checkGoal() {
  float ballX = ball->get_x();
  float ballY = ball->get_y();
  float ballRadius = ball->get_radius();

  // Left goal line
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
  // Right goal line
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

  // Reset after goal delay
  if (ballResetPending && SDL_GetTicks() - goalTime > 1500) {
    resetBall();
    ballResetPending = false;
  }
  if (goalScored && SDL_GetTicks() - goalTime > 3000) {
    goalScored = false;
    goalTeam = 0;
  }

  // End match at 3 goals
  if (team1Score == 3 || team2Score == 3) {
    matchOver = true;
    winnerTeam = (team1Score == 3) ? 1 : 2;
  }
}

void GamePlayWithAIScene::resetBall() {
  ball->set_x(centerX);
  ball->set_y(centerY);
  ball->set_velocity_x(0);
  ball->set_velocity_y(0);
}

void GamePlayWithAIScene::moveAIPlayer(int playerIndex) {
  if (playerIndex < 0 || playerIndex >= 2)
    return;

  float dx = ball->get_x() - team2Players[playerIndex]->get_x();
  float dy = ball->get_y() - team2Players[playerIndex]->get_y();
  float threshold =
      ball->get_radius() + team2Players[playerIndex]->get_radius();
  float dis = std::sqrt(dx * dx + dy * dy);

  if (dis > threshold) {
    if (dx > 0)
      team2Players[playerIndex]->move(SDL_SCANCODE_RIGHT, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
    else if (dx < 0)
      team2Players[playerIndex]->move(SDL_SCANCODE_LEFT, fieldX, fieldY,
                                      fieldWidth, fieldHeight);

    if (dy > 0)
      team2Players[playerIndex]->move(SDL_SCANCODE_DOWN, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
    else if (dy < 0)
      team2Players[playerIndex]->move(SDL_SCANCODE_UP, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
  } else {
    float target_dy =
        this->ground->leftGoal.y - team2Players[playerIndex]->get_y();
    team2Players[playerIndex]->move(SDL_SCANCODE_LEFT, fieldX, fieldY,
                                    fieldWidth, fieldHeight);
    if (target_dy > this->ground->leftGoal.h) {
      team2Players[playerIndex]->move(SDL_SCANCODE_DOWN, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
    } else if (target_dy < -this->ground->leftGoal.h) {
      team2Players[playerIndex]->move(SDL_SCANCODE_UP, fieldX, fieldY,
                                      fieldWidth, fieldHeight);
    }
  }
}

int GamePlayWithAIScene::findBestAIPlayer() {
  float ballX = ball->get_x();
  float ballY = ball->get_y();

  float dist0 = sqrt(pow(team2Players[0]->get_x() - ballX, 2) +
                     pow(team2Players[0]->get_y() - ballY, 2));
  float dist1 = sqrt(pow(team2Players[1]->get_x() - ballX, 2) +
                     pow(team2Players[1]->get_y() - ballY, 2));

  return (dist0 < dist1) ? 0 : 1;
}

void GamePlayWithAIScene::resetBall() {
  ball->set_x(centerX);
  ball->set_y(centerY);
  ball->set_velocity_x(0);
  ball->set_velocity_y(0);
}
