#include "menuScreen.h"

menuScreen::menuScreen(terminalCtrl &terminal)
    : Canvas(terminal), header(canvasWidth, canvasHeight, terminal),
      menuOptions(canvasWidth, canvasHeight, terminal), isRendered(false) {

  std::vector<std::string> menuOptionList = {"\tStart Typing Test",
                                             "\tSelect Options", "\tQuit"};
  menuOptionsNum = menuOptionList.size();

  headerStartY = canvasY + 1;
  headerStartX = canvasX + 1;

  headerWidth = canvasWidth - 2;
  headerHeight = 5;

  menuOptionsStartY = headerStartY + 1; // there's border for the main canvas
  menuOptionsStartX = canvasX + 1;

  menuOptionsWidth = headerWidth;
  menuOptionsHeight = menuOptionsNum + 2; // for top and bottom borders
}

void menuScreen::clear() { spdlog::info("Cleared Main Screen"); }

void menuScreen::render(state_t &state) {
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

void menuScreen::update(state_t &state) {}

selectedMenuOption menuScreen::updateSelection() {
  int currentRow = 0, currentCol = 0;
  terminal.getCurrentCursorPosition(currentRow, currentCol);
  int menuOptsX = menuOptionsStartX + 2;
  int menuOptsPos =
      menuOptionsStartY + (menuOptionsHeight - menuOptionList.size() - 1);
  int selectedMenuOption = (menuOptsPos + currentRow) % 3;

  menuOptions.erase();
  menuOptions.drawBoxWithText(
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth, menuOptionsHeight,
      getMenuString(0), false, borderShape::SHARP_SINGLE, (char *)WHITE,
      (char *)WHITE, false);

  terminal.moveCursor(menuOptsPos + selectedMenuOption, menuOptsX);
  if (selectedMenuOption == 0) {
    return selectedMenuOption::START;
  } else if (selectedMenuOption == 1) {
    return selectedMenuOption::SETTINGS;
  } else {
    return selectedMenuOption::QUIT;
  }
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
