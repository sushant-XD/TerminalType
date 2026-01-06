#include "ui/screens/menuScreen.h"

menuScreen::menuScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      menuOptions(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(MenuOpts::START),
      layout(canvasX, canvasY, canvasWidth, canvasHeight, 3) {

  options = {"\tStart Typing Test", "\tSelect Options", "\tQuit"};

  spdlog::debug("Menu layout - Header: ({},{}) {}x{}, Options: ({},{}) {}x{}",
                layout.header.x, layout.header.y, layout.header.width,
                layout.header.height, layout.options.x, layout.options.y,
                layout.options.width, layout.options.height);
}

void menuScreen::clear() {
  header.erase();
  menuOptions.erase();
  isRendered = false;
}

void menuScreen::render(State &state) {
  if (isRendered)
    return;

  clear();

  std::string headerTitle = "Terminal Typing Test";

  header.drawBoxWithText(layout.header.x, layout.header.y, layout.header.width,
                         layout.header.height, headerTitle, true,
                         borderShape::SHARP_SINGLE, (char *)WHITE,
                         (char *)WHITE, true);
  drawMenuOptions();
  // move cursor to position
  terminal.moveCursor(layout.options.y + 1, layout.options.x + 2);
  currentSelected = MenuOpts::START;
  terminal.hideCursor();
  isRendered = true;
}

void menuScreen::update(const State &state) {
  render(const_cast<State &>(state));
  terminal.hideCursor();
}

MenuOpts menuScreen::updateSelection(bool up) {
  terminal.hideCursor();

  int current = static_cast<int>(currentSelected);
  int numOptions = static_cast<int>(options.size());

  if (up) {
    current = (current - 1 + numOptions) % numOptions;
  } else {
    current = (current + 1) % numOptions;
  }

  currentSelected = static_cast<MenuOpts>(current);

  // Redraw options with new selection
  drawMenuOptions();

  spdlog::info("Selected menu option: {}", current);
  terminal.hideCursor();

  return currentSelected;
}

void menuScreen::drawMenuOptions() {
  menuOptions.erase();

  std::string optionsText =
      formatOptionsWithHighlight(options, static_cast<int>(currentSelected));

  menuOptions.drawBoxWithText(
      layout.options.x, layout.options.y, layout.options.width,
      layout.options.height, optionsText,
      false, // not centered
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE,
      false // not static
  );
}

std::string
menuScreen::formatOptionsWithHighlight(const std::vector<std::string> &opts,
                                       int selectedIndex) {
  std::string result;
  for (size_t i = 0; i < opts.size(); ++i) {
    if (i == selectedIndex) {
      result += "> " + opts[i]; // Highlight with arrow
    } else {
      result += "  " + opts[i];
    }
    if (i < opts.size() - 1) {
      result += "\n";
    }
  }
  return result;
}
