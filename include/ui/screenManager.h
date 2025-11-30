#pragma once
#include "ui/screens/canvas.h"
#include "ui/screens/gameScreen.h"
#include "ui/screens/menuScreen.h"
#include <memory>

class screenManager {
private:
  terminalCtrl &terminal;
  bool needRender;
  testState currentScreen;

  std::unique_ptr<gameScreen> game;
  std::unique_ptr<menuScreen> menu;
  // std::unique_ptr<resultScreen> result;

public:
  screenManager(terminalCtrl &term);
  ~screenManager();

  void switchToScreen(testState newScreen);
  void render(const state_t &state);
  void update(const state_t &state);
  void updateStats(state_t &state);
  selectedMenuOption updateMenuSelection(bool up);

  void clearCurrentScreen();
  void clearTerminal();
  void markForRender();
  bool needsRender() const;
  testState getCurrentScreen() const;
};
