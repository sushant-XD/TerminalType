#include "ui/screenManager.h"
#include <spdlog/spdlog.h>

screenManager::screenManager(terminalCtrl &term)
    : terminal(term), needRender(true), currentScreen(testState::MENU),
      game(nullptr), menu(nullptr) {
  spdlog::debug("screenManager constructor called");
  // Initialize with menu screen
  menu = std::make_unique<menuScreen>(terminal);
}

screenManager::~screenManager() {
  spdlog::debug("screenManager destructor called");
  clearTerminal();
}

void screenManager::switchToScreen(testState newScreen) {
  spdlog::info("Switching to screen: {}", static_cast<int>(newScreen));

  // Clear current screen if different
  if (currentScreen != newScreen) {
    clearCurrentScreen();
  }

  currentScreen = newScreen;

  // Initialize the appropriate screen
  switch (newScreen) {
  case testState::MENU:
    if (!menu) {
      menu = std::make_unique<menuScreen>(terminal);
    }
    needRender = true;
    break;

  case testState::RUNNING:
    if (!game) {
      game = std::make_unique<gameScreen>(terminal);
    }
    needRender = true;
    break;

  case testState::RESULTS:
    // Results can reuse game screen or have its own
    needRender = true;
    break;

  case testState::SETTINGS:
    // Settings screen not yet implemented
    needRender = true;
    break;

  default:
    spdlog::error("Unknown screen state: {}", static_cast<int>(newScreen));
    break;
  }
}

void screenManager::render(const state_t &state) {
  spdlog::debug("Rendering screen: {}", static_cast<int>(currentScreen));

  switch (currentScreen) {
  case testState::MENU:
    if (menu) {
      menu->render(state);
    }
    break;

  case testState::RUNNING:
    if (game) {
      game->render(state);
    }
    break;

  case testState::RESULTS:
    if (game) {
      game->render(state);
    }
    break;

  case testState::SETTINGS:
    // TODO: Implement settings screen rendering
    spdlog::warn("Settings screen rendering not yet implemented");
    break;

  default:
    spdlog::error("Cannot render unknown screen: {}",
                  static_cast<int>(currentScreen));
    break;
  }

  needRender = false;
}

void screenManager::update(const state_t &state) {
  switch (currentScreen) {
  case testState::MENU:
    if (menu) {
      menu->update(state);
    }
    break;

  case testState::RUNNING:
    if (game) {
      game->update(state);
    }
    break;

  case testState::RESULTS:
    if (game) {
      game->update(state);
    }
    break;

  case testState::SETTINGS:
    // TODO: Implement settings screen update
    break;

  default:
    spdlog::error("Cannot update unknown screen: {}",
                  static_cast<int>(currentScreen));
    break;
  }
}

void screenManager::updateStats(state_t &state) {
  if (currentScreen == testState::RUNNING && game) {
    game->updateStats(state);
  }
}

selectedMenuOption screenManager::updateMenuSelection(bool up) {
  if (currentScreen == testState::MENU && menu) {
    return menu->updateSelection(up);
  }
  spdlog::warn("updateMenuSelection called but not in MENU screen");
  return selectedMenuOption::START; // Default return
}

void screenManager::clearCurrentScreen() {
  spdlog::debug("Clearing current screen: {}", static_cast<int>(currentScreen));

  switch (currentScreen) {
  case testState::MENU:
    if (menu) {
      menu->clear();
    }
    break;

  case testState::RUNNING:
  case testState::RESULTS:
    if (game) {
      game->clear();
    }
    break;

  case testState::SETTINGS:
    // TODO: Clear settings screen
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

testState screenManager::getCurrentScreen() const { return currentScreen; }
