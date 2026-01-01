#include "ui/screens/resultScreen.h"

resultScreen::resultScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      header(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      statsBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      optionsBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      isRendered(false), currentSelected(ResultOpts::MENU) {

  optionsList = {"\tReturn to Menu", "\tRestart Test", "\tQuit"};
  optionsNum = optionsList.size();

  // Header dimensions
  headerStartX = canvasX + 1;
  headerStartY = canvasY + 1;
  headerWidth = canvasWidth - 2;
  headerHeight = 3;

  // Stats box dimensions
  statsStartX = canvasX + 1;
  statsStartY = headerStartY + headerHeight + 1;
  statsWidth = canvasWidth - 2;
  statsHeight = 8;

  // Options box dimensions
  optionsStartX = canvasX + 1;
  optionsStartY = statsStartY + statsHeight + 1;
  optionsWidth = canvasWidth - 2;
  optionsHeight = optionsNum + 2; // for top and bottom borders

  spdlog::info("Results screen initialized");
  spdlog::info("Number of result options: {}", optionsNum);
  spdlog::info("Options Starting Position: {},{}\n Options Size: {}x{}",
               optionsStartX, optionsStartY, optionsWidth, optionsHeight);
}

resultScreen::~resultScreen() { clear(); }

void resultScreen::render(State &state) {
  if (isRendered) {
    return;
  }

  clear();

  // Draw header
  std::string headerText = "Test Results";
  header.drawBoxWithText(headerStartX, headerStartY, headerWidth, headerHeight,
                         headerText, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);

  // Build stats content
  std::string statsContent =
      "Words Per Minute: " + std::to_string(state.result.netWPM) + "\n" +
      "Accuracy: " + std::to_string(state.result.accuracy) + "%\n" +
      "Correct Characters: " + std::to_string(state.totalCorrect) + "\n" +
      "Incorrect Characters: " +
      std::to_string(state.totalPressed - state.totalCorrect) + "\n" +
      "Total Characters: " + std::to_string(state.totalPressed);

  // Draw stats box
  statsBox.drawBoxWithText(statsStartX, statsStartY, statsWidth, statsHeight,
                           statsContent, false, borderShape::SHARP_SINGLE,
                           (char *)WHITE, (char *)WHITE, true);

  // Draw options box with current selection
  optionsBox.drawBoxWithText(
      optionsStartX, optionsStartY, optionsWidth, optionsHeight,
      selectOptionInList(optionsList, static_cast<int>(currentSelected)), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  // Position cursor at first option
  terminal.moveCursor(optionsStartY + 1, optionsStartX + 2);

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
      optionsStartX, optionsStartY, optionsWidth, optionsHeight,
      selectOptionInList(optionsList, static_cast<int>(currentSelected)), false,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  spdlog::info("Selected Results Option: {}",
               static_cast<int>(currentSelected));
  return currentSelected;
}
