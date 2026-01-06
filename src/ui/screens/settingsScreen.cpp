#include "ui/screens/settingsScreen.h"

settingsScreen::settingsScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      settingsOptions(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(SettingOption::TIME),
      layout(canvasX, canvasY, canvasWidth, canvasHeight, 3) {

  settingOptionList = {"Time", "Level", "Back to Menu"};
  settingOptionsNum = settingOptionList.size();

  spdlog::info("Settings screen initialized with {} options",
               settingOptionsNum);
  terminal.hideCursor();
}

void settingsScreen::clear() {
  header.erase();
  settingsOptions.erase();
  isRendered = false;
  spdlog::info("Cleared Settings Screen");
}

void settingsScreen::render(State &state) {
  if (isRendered)
    return;

  clear();

  std::string headerTitle = "Settings";
  header.drawBoxWithText(layout.header.x, layout.header.y, layout.header.width,
                         layout.header.height, headerTitle, true,
                         borderShape::SHARP_SINGLE, (char *)WHITE,
                         (char *)WHITE, true);

  settingsOptions.drawBoxWithText(
      layout.options.x, layout.options.y, layout.options.width,
      layout.options.height, getSettingsString(state), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  terminal.moveCursor(layout.options.y + 1, layout.options.x + 2);
  isRendered = true;
  terminal.hideCursor();
}

void settingsScreen::update(const State &state) {
  settingsOptions.erase();
  settingsOptions.drawBoxWithText(
      layout.options.x, layout.options.y, layout.options.width,
      layout.options.height, getSettingsString(state), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);
  terminal.hideCursor();
}

SettingOption settingsScreen::updateSelection(bool up) {
  terminal.hideCursor();

  if (up) {
    int newOption = static_cast<int>(currentSelected) - 1;
    if (newOption < 0) {
      newOption = settingOptionsNum - 1;
    }
    currentSelected = static_cast<SettingOption>(newOption);
  } else {
    currentSelected = static_cast<SettingOption>(
        (static_cast<int>(currentSelected) + 1) % settingOptionsNum);
  }

  spdlog::info("Selected Setting Option: {}",
               static_cast<int>(currentSelected));
  terminal.hideCursor();
  return currentSelected;
}

std::string settingsScreen::getSettingsString(const State &state) {
  std::string result;

  for (int i = 0; i < settingOptionsNum; i++) {
    // Add selection indicator
    if (i == static_cast<int>(currentSelected)) {
      result += "\t> ";
    } else {
      result += "\t  ";
    }

    // Add option name and current value
    switch (static_cast<SettingOption>(i)) {
    case SettingOption::TIME:
      result += "Time: " + std::to_string(state.config.time) + "s";
      result += "  (Press LEFT/RIGHT to change)";
      break;

    case SettingOption::LEVEL:
      result += "Level: ";
      switch (state.config.level) {
      case Level::EASY:
        result += "Easy";
        break;
      case Level::MEDIUM:
        result += "Medium";
        break;
      case Level::HARD:
        result += "Hard";
        break;
      }
      result += "  (Press LEFT/RIGHT to change)";
      break;

    case SettingOption::BACK:
      result += settingOptionList[i];
      break;
    }

    result += "\n\n";
  }

  return result;
}
