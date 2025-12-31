#include "ui/screens/menuScreen.h"

menuScreen::menuScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      menuOptions(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(MenuOpts::START) {

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

void menuScreen::clear() {
  header.erase();
  menuOptions.erase();
  isRendered = false;
  spdlog::info("Cleared Main Screen");
}

void menuScreen::render(State &state) {
  if (isRendered)
    return;

  clear();

  std::string headerTitle = "Terminal Typing Test";

  header.drawBoxWithText(headerStartX, headerStartY, headerWidth, headerHeight,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);

  menuOptions.drawBoxWithText(
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth, menuOptionsHeight,
      selectOptionInList(menuOptionList, 0), false, borderShape::SHARP_SINGLE,
      (char *)WHITE, (char *)WHITE, false);

  // move cursor to position
  terminal.moveCursor(menuOptionsStartY + 1, menuOptionsStartX + 2);
  // terminal.hideCursor();
  isRendered = true;
}

void menuScreen::update(const State &state) {
  render(const_cast<State &>(state));
}

MenuOpts menuScreen::updateSelection(bool up) {
  terminal.hideCursor();
  if (up) {
    int newOption = static_cast<int>(currentSelected) - 1;
    if (newOption < 0) {
      newOption = menuOptionsNum - 1;
    }
    currentSelected = static_cast<MenuOpts>(newOption % menuOptionsNum);
  } else {
    currentSelected = static_cast<MenuOpts>(
        (static_cast<int>(currentSelected) + 1) % menuOptionsNum);
  }
  menuOptions.erase();
  menuOptions.drawBoxWithText(
      menuOptionsStartX, menuOptionsStartY, menuOptionsWidth, menuOptionsHeight,
      selectOptionInList(menuOptionList, static_cast<int>(currentSelected)),
      false, borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);
  spdlog::info("Selected Menu Option: {}", static_cast<int>(currentSelected));
  return currentSelected;
}
