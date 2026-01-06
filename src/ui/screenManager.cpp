#include "ui/screenManager.h"
#include <spdlog/spdlog.h>

screenManager::screenManager(terminalCtrl &term)
    : terminal(term), needRender(true), currentScreen(TestState::MENU),
      game(nullptr), menu(nullptr), result(nullptr) {
  spdlog::debug("screenManager constructor called");
  // Initialize with menu screen
  clearTerminal();
  menu = std::make_unique<menuScreen>(terminal);
}

screenManager::~screenManager() {
  spdlog::debug("screenManager destructor called");
  clearTerminal();
}

void screenManager::switchToScreen(TestState newScreen) {
  spdlog::info("Switching to screen: {}", static_cast<int>(newScreen));

  // Clear current screen if different
  if (currentScreen != newScreen) {
    clearCurrentScreen();
  }

  currentScreen = newScreen;

  // Initialize the appropriate screen
  switch (newScreen) {
  case TestState::MENU:
    if (!menu) {
      menu = std::make_unique<menuScreen>(terminal);
    }
    needRender = true;
    break;

  case TestState::RUNNING:
    if (!game) {
      game = std::make_unique<gameScreen>(terminal);
    }
    needRender = true;
    break;

  case TestState::RESULTS:
    if (!result) {
      result = std::make_unique<resultScreen>(terminal);
    }
    needRender = true;
    break;

  case TestState::SETTINGS:
    if (!settings) {
      settings = std::make_unique<settingsScreen>(terminal);
    }
    needRender = true;
    break;

  default:
    spdlog::error("Unknown screen state: {}", static_cast<int>(newScreen));
    break;
  }
}

void screenManager::render(State &state) {
  spdlog::debug("Rendering screen: {}", static_cast<int>(currentScreen));

  switch (currentScreen) {
  case TestState::MENU:
    if (menu) {
      menu->render(state);
    }
    break;

  case TestState::RUNNING:
    if (game) {
      game->render(state);
    }
    break;

  case TestState::RESULTS:
    if (result) {
      result->render(state);
    }
    break;

  case TestState::SETTINGS:
    if (settings) {
      settings->render(state);
    }
    break;

  default:
    spdlog::error("Cannot render unknown screen: {}",
                  static_cast<int>(currentScreen));
    break;
  }

  needRender = false;
}

void screenManager::update(const State &state) {
  switch (currentScreen) {
  case TestState::MENU:
    if (menu) {
      menu->update(state);
    }
    break;

  case TestState::RUNNING:
    if (game) {
      game->update(state);
    }
    break;

  case TestState::RESULTS:
    if (result) {
      result->update(state);
    }
    break;

  case TestState::SETTINGS:
    if (settings) {
      settings->update(state);
    }
    break;

  default:
    spdlog::error("Cannot update unknown screen: {}",
                  static_cast<int>(currentScreen));
    break;
  }
}

void screenManager::updateStats(State &state) {
  if (currentScreen == TestState::RUNNING && game) {
    game->updateStats(state);
  }
}

MenuOpts screenManager::updateMenuSelection(bool up) {
  if (currentScreen == TestState::MENU && menu) {
    return menu->updateSelection(up);
  }
  spdlog::warn("updateMenuSelection called but not in MENU screen");
  return MenuOpts::START; // Default return
}

ResultOpts screenManager::updateResultsSelection(bool up) {
  if (currentScreen == TestState::RESULTS && result) {
    return result->updateSelection(up);
  }
  spdlog::warn("updateResultsSelection called but not in RESULTS screen");
  return ResultOpts::MENU; // Default return
}

SettingOption screenManager::updateSettingsSelection(bool up) {
  if (currentScreen == TestState::SETTINGS && settings) {
    return settings->updateSelection(up);
  }
  spdlog::warn("updateSettingsSelection called but not in SETTINGS screen");
  return SettingOption::TIME;
}

void screenManager::modifySettingValue(State &state, bool increase) {
  if (currentScreen != TestState::SETTINGS || !settings) {
    return;
  }

  SettingOption current = settings->getCurrentSelection();

  switch (current) {
  case SettingOption::TIME:
    if (increase) {
      state.config.time = std::min(state.config.time + 10, 300); // Max 300s
    } else {
      state.config.time = std::max(state.config.time - 10, 10); // Min 10s
    }
    spdlog::info("Time changed to: {}", state.config.time);
    break;

  case SettingOption::LEVEL:
    if (increase) {
      int level = static_cast<int>(state.config.level);
      level = (level + 1) % 3; // Cycle through 0, 1, 2
      state.config.level = static_cast<Level>(level);
    } else {
      int level = static_cast<int>(state.config.level);
      level = (level - 1 + 3) % 3;
      state.config.level = static_cast<Level>(level);
    }
    spdlog::info("Level changed to: {}", static_cast<int>(state.config.level));
    break;

  case SettingOption::BACK:
    // Do nothing for back option
    break;
  }

  settings->update(state);
}

void screenManager::clearCurrentScreen() {
  spdlog::debug("Clearing current screen: {}", static_cast<int>(currentScreen));

  switch (currentScreen) {
  case TestState::MENU:
    if (menu) {
      menu->clear();
    }
    break;

  case TestState::RUNNING:
    if (game) {
      game->clear();
    }
    break;
  case TestState::RESULTS:
    if (result) {
      result->clear();
    }
    break;
  case TestState::SETTINGS:
    if (settings) {
      settings->clear();
    }
    break;

  default:
    break;
  }
}

void screenManager::clearTerminal() {
  spdlog::debug("Clearing entire terminal");
  terminal.writeToTerminal((char *)CLS, strlen(CLS));
  terminal.moveCursor(0, 0);
}

void screenManager::markForRender() {
  needRender = true;
  spdlog::debug("Screen marked for re-render");
}

bool screenManager::needsRender() const { return needRender; }

TestState screenManager::getCurrentScreen() const { return currentScreen; }
