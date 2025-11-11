#include "design.h"
#include <ostream>
#include <spdlog/spdlog.h>
#include <string>
#define BORDER_COLOR BCYAN
#define DIVIDER_COLOR BBLACK
#define TEXT_COLOR WHITE
#define INPUT_TEXT_COLOR WHITE
#define ERROR_TEXT_COLOR RED
#define CORRECT_TEXT_COLOR GREEN

using namespace std::chrono;

screenState::screenState(terminalCtrl &terminalManager)
    : terminalManager(terminalManager),
      mainScreen(terminalManager.getTerminalWidth(),
                 terminalManager.getTerminalWidth(), terminalManager),
      header(terminalManager.getTerminalWidth(),
             terminalManager.getTerminalWidth(), terminalManager),
      stats(terminalManager.getTerminalWidth(),
            terminalManager.getTerminalWidth(), terminalManager),
      mainTextBox(terminalManager.getTerminalWidth(),
                  terminalManager.getTerminalWidth(), terminalManager) {
  clearTerminal();
  terminalHeight = terminalManager.getTerminalHeight();
  terminalWidth = terminalManager.getTerminalWidth();

  windowWidth = terminalWidth - 20;
  windowHeight = terminalHeight - 5;

  windowStartRow = 1;
  windowStartCol = 1;

  headerStartRow = windowStartRow + 1;
  StatsStartRow = headerStartRow + 3;
  displayTextStartRow = StatsStartRow + 3;
  spdlog::debug("Height of terminal: {},windowWidth of terminal: {}",
                terminalHeight, terminalWidth);
  spdlog::debug(
      "Height of Window: {}windowWidth of Window: {} Start Position: {},{}",
      windowHeight, windowWidth, windowStartRow, windowStartCol);
}

screenState::~screenState() {}

void screenState::clearTerminal() {
  terminalManager.writeToTerminal((char *)CLS, strlen(CLS));
}

void screenState::updateStats(state_t &state) {
  if (state.isRunning) {
    auto elapsed =
        duration_cast<seconds>(steady_clock::now() - state.startTime);
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
}

// Complete gradient box render function
void screenState::renderGradientBox(state_t &state) {
  clearTerminal();
  spdlog::info("terminal cleared");

  std::string headerTitle = "Terminal Typing Test";
  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";

  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  mainScreen.drawBox(windowStartCol, windowStartRow, windowWidth, windowHeight,
                     true, borderShape::SHARP_SINGLE, (char *)BLUE, true);
  header.drawBoxWithText(windowStartCol + 1, headerStartRow, windowWidth - 2, 3,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);
  stats.drawBoxWithText(windowStartCol + 1, StatsStartRow, windowWidth - 2, 3,
                        statsContent, true, borderShape::SHARP_SINGLE,
                        (char *)WHITE, (char *)WHITE, false);
  mainTextBox.drawBoxWithText(
      windowStartCol + 1, displayTextStartRow, windowWidth - 2, 6, displayText,
      true, borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);
  spdlog::info("Render Gradient Setup complete");
}

void screenState::renderTextProgress(state_t &state) {
  int displayIndex;
  if (state.currentKeyStatus == keyStroke::BACKSPACE) {
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
  } else if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    mainTextBox.updateText((char *)&state.targetSequence[displayIndex],
                           currentTextRow, currentTextCol, 1, (char *)WHITE);
  }
  spdlog::info("Text Render progress complete");
}

void screenState::testComplete() {}

void screenState::get_and_print_result(state_t &state) {
  clearTerminal();
  updateStats(state);
}

void screenState::appendToBuffer(std::vector<char> &buffer, const char *data) {
  if (data) {
    size_t len = strlen(data);
    if (len > 0) {
      buffer.insert(buffer.end(), data, data + len);
    }
  }
}

void screenState::appendToBuffer(std::vector<char> &buffer,
                                 const std::string &data) {
  if (!data.empty()) {
    buffer.insert(buffer.end(), data.begin(), data.end());
  }
}
