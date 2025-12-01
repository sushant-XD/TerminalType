#pragma once
#include "ui/screens/canvas.h"
#include "ui/screens/gameScreen.h"
#include "ui/screens/menuScreen.h"
#include "ui/screens/resultScreen.h"
#include "ui/screens/settingsScreen.h"
#include <memory>

class screenManager {
private:
  terminalCtrl &terminal;
  bool needRender;
  TestState currentScreen;

  std::unique_ptr<gameScreen> game;
  std::unique_ptr<menuScreen> menu;
  std::unique_ptr<resultScreen> result;
  std::unique_ptr<settingsScreen> settings;

public:
  screenManager(terminalCtrl &term);
  ~screenManager();

  void switchToScreen(TestState newScreen);
  void render(State &state);
  void update(const State &state);
  void updateStats(State &state);
  MenuOpts updateMenuSelection(bool up);
  ResultOpts updateResultsSelection(bool up);

  SettingOption updateSettingsSelection(bool up);
  void modifySettingValue(State &state, bool increase);

  void clearCurrentScreen();
  void clearTerminal();
  void markForRender();
  bool needsRender() const;
  TestState getCurrentScreen() const;
};
