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
    : terminalManager(terminalManager) {
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

    // Clear the line

    // Recreate the same formatting as drawStats()
    int padding = 4;
    int innerPadding = 6;
    int actualWidth = windowWidth - (padding * 2);

    std::string left =
        "Time Remaining: " + std::to_string(state.remainingTimeSeconds) + "s";
    std::string right = "WPM: " + std::to_string(wpm);
    std::string middle = "|";

    int totalContentWidth =
        left.size() + middle.size() + right.size() + innerPadding * 2;
    int sideSpace = (actualWidth - totalContentWidth) / 2;

    // Build the line exactly like drawStats does
    std::string statsLine = std::string(sideSpace, ' ') + left +
                            std::string(innerPadding, ' ') + middle +
                            std::string(innerPadding, ' ') + right;

    int displayIndex = state.charCount;
    int rem = displayIndex / windowWidth;
    int inputRow = 6 + rem;
    int inputCol = displayIndex % windowWidth;

    // Move to stats line
    terminalManager.hideCursor();
    terminalManager.moveCursor(StatsStartRow, 0);
    terminalManager.writeToTerminal((char *)"\033[2K", 4); // Clear entire line
    spdlog::info("Moved Cursor to position: {} 0", StatsStartRow);
    terminalManager.writeToTerminal((char *)statsLine.c_str(),
                                    statsLine.length());
    terminalManager.writeToTerminal((char *)CRESET, strlen(CRESET));
    terminalManager.moveCursor(inputRow, inputCol);
    terminalManager.showCursor();
  }
}

// Complete gradient box render function
void screenState::renderGradientBox(state_t &state) {
  clearTerminal();
  spdlog::info("terminal cleared");

  std::string headerTitle = "Terminal Typing Test";
  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";

  uiWidget mainScreen(terminalWidth, terminalHeight, terminalManager);
  uiWidget header(windowWidth, windowHeight, terminalManager);
  uiWidget stats(windowWidth, windowHeight, terminalManager);
  uiWidget mainTextBox(windowWidth, windowHeight, terminalManager);

  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  mainScreen.drawBox(windowStartCol, windowStartRow, windowWidth, windowHeight,
                     true, borderShape::SHARP_SINGLE, (char *)BLUE, false);
  header.drawBoxWithText(windowStartCol + 1, headerStartRow, windowWidth - 2, 3,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);
  stats.drawBoxWithText(windowStartCol + 1, StatsStartRow, windowWidth - 2, 3,
                        statsContent, true, borderShape::SHARP_SINGLE,
                        (char *)WHITE, (char *)WHITE, true);
  mainTextBox.drawBoxWithText(
      windowStartCol + 1, displayTextStartRow, windowWidth - 2,
      displayText.length() / (windowWidth - 2), displayText, true,
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, true);
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

  int rem = displayIndex / StatsStartRow;
  int row = 6 + rem;
  int col = displayIndex % windowWidth;
  terminalManager.moveCursor(row, col);
  spdlog::info("Moved to position {} {}", row, col);
  if (state.currentKeyStatus == keyStroke::CORRECT) {
    terminalManager.writeToTerminal((char *)GREEN, strlen(GREEN));
  } else if (state.currentKeyStatus == keyStroke::INCORRECT) {
    terminalManager.writeToTerminal((char *)RED, strlen(RED));
  } else if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    terminalManager.writeToTerminal((char *)WHITE, strlen(WHITE));
  }
  terminalManager.writeToTerminal((char *)&state.targetSequence[displayIndex],
                                  1);
  terminalManager.writeToTerminal((char *)CRESET, strlen(CRESET));
  if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    terminalManager.moveCursor(row, col);
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
