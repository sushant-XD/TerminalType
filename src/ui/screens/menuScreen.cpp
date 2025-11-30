#include "ui/screens/menuScreen.h"

menuScreen::menuScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      menuOptions(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(selectedMenuOption::START) {

  menuOptionList = {"\tStart Typing Test", "\tSelect Options", "\tQuit"};
  menuOptionsNum = menuOptionList.size();

  headerStartY = canvasY + 1;
  headerStartX = canvasX + 1;

  headerWidth = canvasWidth - 2;
  headerHeight = 3;

  menuOptionsStartY =
      headerStartY + headerHeight; // there's border for the main canvas
  menuOptionsStartX = canvasX + 1;

  menuOptionsWidth = headerWidth;
  menuOptionsHeight = menuOptionsNum + 2; // for top and bottom borders

  spdlog::info("Number of menu options: {}", menuOptionsNum);
  spdlog::info("Header Starting Position: {},{}\n Header Size: {}x{}",
               headerStartX, headerStartY, headerWidth, headerHeight);
  spdlog::info(
      "Menu Options Starting Position: {},{}\n Menu Options Size: {}x{}",
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth,
      menuOptionsHeight);
}

void menuScreen::clear() { spdlog::info("Cleared Main Screen"); }

void menuScreen::render(const state_t &state) {
  if (isRendered)
    return;

  clear();

  std::string headerTitle = "Terminal Typing Test";

  header.drawBoxWithText(headerStartX, headerStartY, headerWidth, headerHeight,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);

  menuOptions.drawBoxWithText(
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth, menuOptionsHeight,
      getMenuString(0), false, borderShape::SHARP_SINGLE, (char *)WHITE,
      (char *)WHITE, false);

  // move cursor to position
  terminal.moveCursor(menuOptionsStartY + 1, menuOptionsStartX + 2);
  // terminal.hideCursor();
  isRendered = true;
}

void menuScreen::update(const state_t &state) { render(state); }

selectedMenuOption menuScreen::updateSelection(bool up) {
  terminal.hideCursor();
  if (up) {
    int newOption = static_cast<int>(currentSelected) - 1;
    if (newOption < 0) {
      newOption = menuOptionsNum - 1;
    }
    currentSelected =
        static_cast<selectedMenuOption>(newOption % menuOptionsNum);
  } else {
    currentSelected = static_cast<selectedMenuOption>(
        (static_cast<int>(currentSelected) + 1) % menuOptionsNum);
  }
  menuOptions.erase();
  menuOptions.drawBoxWithText(
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth, menuOptionsHeight,
      getMenuString(static_cast<int>(currentSelected)), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);
  spdlog::info("Selected Menu Option: {}", static_cast<int>(currentSelected));
  return currentSelected;
}

std::string menuScreen::getMenuString(int option) {
  std::vector<std::string> options = menuOptionList;

  if (option >= 0 && option < options.size()) {
    options[option] = "\t> " + options[option];
  }

  std::string menuString;
  for (const auto &line : options) {
    menuString += line + "\n";
  }
  return menuString;
}
