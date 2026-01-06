#include "ui/screens/gameScreen.h"

gameScreen::gameScreen(terminalCtrl &terminal)
    : Canvas(terminal),
      stats(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      mainTextBox(canvasWidth + canvasX, canvasHeight + canvasY, terminal),
      layout(canvasX, canvasY, canvasWidth, canvasHeight) {
  isRendered = false;
}

gameScreen::~gameScreen() {}

void gameScreen::render(State &state) {
  if (isRendered)
    return;

  assert(!state.targetSequence.empty());
  clear();

  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";
  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  int innerWidth = layout.textBox.width - 2;
  state.wrappedLines = mainTextBox.wrapText(displayText, innerWidth);

  stats.drawBoxWithText(layout.stats.x, layout.stats.y, layout.stats.width,
                        layout.stats.height, statsContent, true,
                        borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE,
                        false);
  mainTextBox.drawBoxWithText(layout.textBox.x, layout.textBox.y,
                              layout.textBox.width, layout.textBox.height,
                              displayText, false, borderShape::SHARP_SINGLE,
                              (char *)WHITE, (char *)WHITE, false);
  spdlog::info("Render Gradient Setup complete");
}

void gameScreen::update(const State &state) {
  assert(!state.targetSequence.empty());

  // Determine which character index to update based on the key status
  int displayIndex;
  if (state.currentKeyStatus == KeyStroke::BACKSPACE) {
    displayIndex = state.charCount;
  } else if (state.currentKeyStatus == KeyStroke::BACK_WORD) {
    displayIndex = state.charCount;
  } else {
    displayIndex = state.charCount - 1;
  }

  // Validate the index
  if (displayIndex < 0 ||
      displayIndex >= static_cast<int>(state.targetSequence.size())) {
    spdlog::warn("displayIndex {} out of bounds (size: {})", displayIndex,
                 state.targetSequence.size());
    return;
  }

  // Handle different input types
  if (state.currentKeyStatus == KeyStroke::CORRECT) {
    // User typed the correct character - color it green
    updateCharacterColor(displayIndex, state, (char *)GREEN);

  } else if (state.currentKeyStatus == KeyStroke::INCORRECT) {
    // User typed the wrong character - color it red
    updateCharacterColor(displayIndex, state, (char *)RED);

  } else if (state.currentKeyStatus == KeyStroke::BACKSPACE ||
             state.currentKeyStatus == KeyStroke::BACK_WORD) {
    // User deleted characters - reset them to white
    for (int i = 0; i < state.backspaceCount; ++i) {
      int charToReset = state.charCount + i;

      // Validate each character being reset
      if (charToReset < 0 ||
          charToReset >= static_cast<int>(state.targetSequence.size())) {
        spdlog::warn("Backspace charIndex {} out of bounds (size: {})",
                     charToReset, state.targetSequence.size());
        continue;
      }

      // Reset character to default white color
      updateCharacterColor(charToReset, state, (char *)WHITE);
    }
  }
}

void gameScreen::updateStats(State &state) {
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

std::pair<int, int> gameScreen::calculateCharacterPosition(int charIndex,
                                                           const State &state) {
  int targetIndex = 0;

  for (int row = 0; row < state.wrappedLines.size(); row++) {
    const std::string &line = state.wrappedLines[row];

    // Check each character in this wrapped line
    for (int col = 0; col < line.length(); col++) {
      if (targetIndex == charIndex) {
        return {row, col};
      }
      targetIndex++;
    }

    // After each line, skip any spaces in the original text that were consumed
    // during wrapping (spaces between this line and the next)
    while (targetIndex < state.targetSequence.size() &&
           state.targetSequence[targetIndex] == ' ') {
      if (targetIndex == charIndex) {
        // The character we're looking for IS a space that was consumed
        // Place it at the end of the current line
        return {row, static_cast<int>(line.length())};
      }
      targetIndex++;
    }
  }

  // Fallback
  int lastRow = state.wrappedLines.size() - 1;
  int lastCol =
      state.wrappedLines.empty() ? 0 : state.wrappedLines[lastRow].length();
  return {lastRow, lastCol};
}

void gameScreen::updateCharacterColor(int charIndex, const State &state,
                                      char *color) {
  auto [row, col] = calculateCharacterPosition(charIndex, state);

  int absoluteRow = mainTextBox.getTextStartRow() + row;
  int absoluteCol =
      mainTextBox.getTextStartColumn() + col + 1; //  terminal work 1-indexed

  mainTextBox.updateText((char *)&state.targetSequence[charIndex], absoluteRow,
                         absoluteCol, 1, color);
}

void gameScreen::clear() {
  stats.erase();
  mainTextBox.erase();

  isRendered = false;
  spdlog::info("Cleared Game Screen");
}
