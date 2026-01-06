#include "ui/screens/resultScreen.h"

resultScreen::resultScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      statsBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      optionsBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(ResultOpts::MENU),
      layout(canvasX, canvasY, canvasWidth, canvasHeight, 3) {

  optionsList = {"\tReturn to Menu", "\tRestart Test", "\tQuit"};
  optionsNum = optionsList.size();

  terminal.hideCursor();
}

resultScreen::~resultScreen() { clear(); }

void resultScreen::render(State &state) {
  if (isRendered) {
    return;
  }

  clear();
  terminal.hideCursor();

  // Draw header
  std::string headerText = "Test Results";
  header.drawBoxWithText(layout.header.x, layout.header.y, layout.header.width,
                         layout.header.height, headerText, true,
                         borderShape::SHARP_SINGLE, (char *)WHITE,
                         (char *)WHITE, true);

  // Build stats content
  std::string statsContent =
      "Words Per Minute: " + std::to_string(state.result.netWPM) + "\n" +
      "Accuracy: " + std::to_string(state.result.accuracy) + "%\n" +
      "Correct Characters: " + std::to_string(state.totalCorrect) + "\n" +
      "Incorrect Characters: " +
      std::to_string(state.totalPressed - state.totalCorrect) + "\n" +
      "Total Characters: " + std::to_string(state.totalPressed);

  // Draw stats box
  statsBox.drawBoxWithText(layout.stats.x, layout.stats.y, layout.stats.width,
                           layout.stats.height, statsContent, false,
                           borderShape::SHARP_SINGLE, (char *)WHITE,
                           (char *)WHITE, true);

  // Draw options box with current selection
  optionsBox.drawBoxWithText(
      layout.options.x, layout.options.y, layout.options.width,
      layout.options.height,
      selectOptionInList(optionsList, static_cast<int>(currentSelected)), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  // Position cursor at first option (inverted because row, col (y,x))
  terminal.moveCursor(layout.options.y + 1, layout.options.x + 2);

  terminal.hideCursor();
  isRendered = true;
  spdlog::info("Results screen rendered successfully");
}

void resultScreen::update(const State &state) {
  // Results screen is mostly static, updates handled by updateSelection
}

void resultScreen::clear() {
  header.erase();
  statsBox.erase();
  optionsBox.erase();
  isRendered = false;
  currentSelected = ResultOpts::MENU; // Reset selection
  spdlog::info("Cleared Results Screen");
}

ResultOpts resultScreen::updateSelection(bool up) {
  terminal.hideCursor();

  if (up) {
    int newOption = static_cast<int>(currentSelected) - 1;
    if (newOption < 0) {
      newOption = optionsNum - 1;
    }
    currentSelected = static_cast<ResultOpts>(newOption % optionsNum);
  } else {
    currentSelected = static_cast<ResultOpts>(
        (static_cast<int>(currentSelected) + 1) % optionsNum);
  }

  // Redraw options box with new selection
  optionsBox.erase();
  optionsBox.drawBoxWithText(
      layout.options.x, layout.options.y, layout.options.width,
      layout.options.height,
      selectOptionInList(optionsList, static_cast<int>(currentSelected)), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  spdlog::info("Selected Results Option: {}",
               static_cast<int>(currentSelected));
  terminal.hideCursor();
  return currentSelected;
}
