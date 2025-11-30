#include "ui/screens/gameScreen.h"

gameScreen::gameScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      stats(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      mainTextBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal) {

  statsWidth = canvasWidth - 2;
  statsHeight = 3;

  statsStartX = canvasX + 1;
  statsStartY = canvasY + 1;

  mainTextBoxWidth = canvasWidth - 2;
  mainTextBoxHeight = 12;

  mainTextBoxStartX = statsStartX;
  mainTextBoxStartY = statsStartY + statsHeight + 1;

  isRendered = false;
}

gameScreen::~gameScreen() {}

void gameScreen::render(const state_t &state) {
  if (isRendered)
    return;

  clear();

  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";
  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  stats.drawBoxWithText(statsStartX, statsStartY, statsWidth, statsHeight,
                        statsContent, true, borderShape::SHARP_SINGLE,
                        (char *)WHITE, (char *)WHITE, false);
  mainTextBox.drawBoxWithText(mainTextBoxStartX, mainTextBoxStartY,
                              mainTextBoxWidth, mainTextBoxHeight, displayText,
                              true, borderShape::SHARP_SINGLE, (char *)WHITE,
                              (char *)WHITE, false);
  spdlog::info("Render Gradient Setup complete");
}

void gameScreen::update(const state_t &state) {
  int displayIndex;

  if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    displayIndex = state.charCount;
  } else if (state.currentKeyStatus == keyStroke::BACK_WORD) {
    displayIndex = state.charCount;
  } else {
    displayIndex = state.charCount - 1;
  }

  if (displayIndex < 0 ||
      displayIndex >= static_cast<int>(state.targetSequence.size())) {
    spdlog::warn("displayIndex {} out of bounds (size: {})", displayIndex,
                 state.targetSequence.size());
    return;
  }

  int currentTextRow = mainTextBox.getTextStartRow();
  int currentTextCol = mainTextBox.getTextStartColumn() + displayIndex + 1;

  if (state.currentKeyStatus == keyStroke::CORRECT) {
    mainTextBox.updateText((char *)&state.targetSequence[displayIndex],
                           currentTextRow, currentTextCol, 1, (char *)GREEN);
  } else if (state.currentKeyStatus == keyStroke::INCORRECT) {

    mainTextBox.updateText((char *)&state.targetSequence[displayIndex],
                           currentTextRow, currentTextCol, 1, (char *)RED);
  } else if (state.currentKeyStatus == keyStroke::BACKSPACE ||
             state.currentKeyStatus == keyStroke::BACK_WORD) {
    for (int i = 0; i < state.backspaceCount; ++i) {
      // The characters to whitewash start at the new, lower charCount
      displayIndex = state.charCount + i;

      if (displayIndex < 0 ||
          displayIndex >= static_cast<int>(state.targetSequence.size())) {
        spdlog::warn("Backspace displayIndex {} out of bounds (size: {})",
                     displayIndex, state.targetSequence.size());
        continue; // Skip this char if it's out of bounds
      }

      int currentTextCol = mainTextBox.getTextStartColumn() + displayIndex + 1;

      // Update this single character to WHITE
      mainTextBox.updateText((char *)&state.targetSequence[displayIndex],
                             currentTextRow, currentTextCol, 1, (char *)WHITE);
    }
  }
  spdlog::info("Text Render progress complete");
}

void gameScreen::updateStats(state_t &state) {
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now() - state.startTime);
  float timeInMinutes = elapsed.count() / 60.0f;
  int wpm = timeInMinutes > 0 ? (state.correctCount / 5) / timeInMinutes : 0;

  std::string statsContent =
      "Time Remaining: " + std::to_string(state.remainingTimeSeconds) +
      "    |    WPM: " + std::to_string(wpm);
  int StatsStartRow = stats.getTextStartRow();
  int StatsStartCol = stats.getTextStartColumn();
  stats.updateText((char *)statsContent.c_str(), StatsStartRow, StatsStartCol,
                   statsContent.size(), (char *)WHITE);
}

void gameScreen::clear() {}
