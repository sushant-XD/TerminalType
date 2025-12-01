#pragma once
#include "ui/screens/canvas.h"
#include "ui/screens/gameScreen.h"
#include "ui/screens/menuScreen.h"
#include "ui/screens/resultScreen.h"
#include <memory>

class screenManager {
private:
  terminalCtrl &terminal;
  bool needRender;
  TestState currentScreen;

  std::unique_ptr<gameScreen> game;
  std::unique_ptr<menuScreen> menu;
  std::unique_ptr<resultScreen> result;

public:
  screenManager(terminalCtrl &term);
  ~screenManager();

  void switchToScreen(TestState newScreen);
  void render(const State &state);
  void update(const State &state);
  void updateStats(State &state);
  MenuOpts updateMenuSelection(bool up);
  ResultOpts updateResultsSelection(bool up);

  void clearCurrentScreen();
  void clearTerminal();
  void markForRender();
  bool needsRender() const;
  TestState getCurrentScreen() const;
};
