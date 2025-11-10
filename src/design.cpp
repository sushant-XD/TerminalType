#include "design.h"
#include <ostream>
#include <spdlog/spdlog.h>
#define BORDER_COLOR BCYAN
#define DIVIDER_COLOR BBLACK
#define TEXT_COLOR WHITE
#define INPUT_TEXT_COLOR WHITE
#define ERROR_TEXT_COLOR RED
#define CORRECT_TEXT_COLOR GREEN

using namespace std::chrono;

screenState::screenState(terminalCtrl &terminalManager)
    : terminalManager(terminalManager), initialSetupComplete(false) {
  clearTerminal();
  height = terminalManager.getTerminalHeight();
  width = terminalManager.getTerminalWidth();
  spdlog::info("Height of terminal: {}, width of terminal: {}", height, width);
  outerCommonPaddingLR = 4;
  inputBoxPaddingLR = 3;
  linSpacing = 1;
  textStartLine = 6;
  statsRow = 5;
}

screenState::~screenState() {}

void screenState::clearTerminal() {
  terminalManager.writeToTerminal((char *)CLS, strlen(CLS));
}

void screenState::drawHeader(std::string content) {
  int contentLength = content.size();
  int remainingLength = (width - contentLength - 2) / 2; // -2 for borders
  std::vector<char> buffer;
  buffer.reserve(width * 3 + contentLength + 100);
  // Top line
  appendToBuffer(buffer, BORDER_COLOR);
  appendToBuffer(buffer, BoxChars::TOP_LEFT_ROUND);
  for (int i = 0; i < width - 2; i++) {
    appendToBuffer(buffer, BoxChars::HORIZONTAL);
  }
  appendToBuffer(buffer, BoxChars::TOP_RIGHT_ROUND);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, "\r\n"); // Use \r\n for terminal newlines

  // --- Content line ---
  appendToBuffer(buffer, BORDER_COLOR);
  appendToBuffer(buffer, BoxChars::VERTICAL);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, std::string(remainingLength, ' '));
  appendToBuffer(buffer, BWHITE);
  appendToBuffer(buffer, content);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, std::string(remainingLength, ' '));
  appendToBuffer(buffer, BORDER_COLOR);
  appendToBuffer(buffer, BoxChars::VERTICAL);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, "\r\n");

  // --- Divider line ---
  appendToBuffer(buffer, DIVIDER_COLOR);
  appendToBuffer(buffer, BoxChars::T_LEFT);
  for (int i = 0; i < width - 2; i++) {
    appendToBuffer(buffer, BoxChars::HORIZONTAL);
  }
  appendToBuffer(buffer, BoxChars::T_RIGHT);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, "\r\n");

  terminalManager.moveCursor(0, 0);
  spdlog::info("Moved Cursor to position 0 0");
  terminalManager.writeToTerminal(buffer.data(), buffer.size());
}

void screenState::drawStats(int timeRemaining, int speed, int level) {
  int padding = 4;
  int innerPadding = 6;
  int actualWidth = width - (padding * 2);

  std::string left = "Time Remaining: " + std::to_string(timeRemaining) + "s";
  std::string right = "Level: " + std::to_string(level);
  std::string middle = "|";

  int totalContentWidth =
      left.size() + middle.size() + right.size() + innerPadding * 2;
  int sideSpace = (actualWidth - totalContentWidth) / 2;

  std::vector<char> buffer;

  // Build the strings first
  std::string sideSpaceStr = std::string(sideSpace, ' ');
  std::string innerPadStr = std::string(innerPadding, ' ');

  // Append to the vector
  appendToBuffer(buffer, sideSpaceStr);
  appendToBuffer(buffer, left);
  appendToBuffer(buffer, innerPadStr);
  appendToBuffer(buffer, middle);
  appendToBuffer(buffer, innerPadStr);
  appendToBuffer(buffer, right);
  appendToBuffer(buffer, "\r\n");

  terminalManager.moveCursor(statsRow, 0);
  spdlog::info("Moved Cursor to position: {} 0", statsRow);
  terminalManager.writeToTerminal(buffer.data(), buffer.size());
}

void screenState::drawEmptyLine() {
  std::vector<char> buffer;
  buffer.reserve(width + 20); // Pre-allocate

  appendToBuffer(buffer, BORDER_COLOR);
  appendToBuffer(buffer, BoxChars::VERTICAL);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, std::string(width - 2, ' '));
  appendToBuffer(buffer, BORDER_COLOR);
  appendToBuffer(buffer, BoxChars::VERTICAL);
  appendToBuffer(buffer, CRESET);
  appendToBuffer(buffer, "\r\n");

  terminalManager.moveCursor(3, 0);
  spdlog::info("Moved Cursor to position: 3 0");
  terminalManager.writeToTerminal(buffer.data(), buffer.size());
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
    int actualWidth = width - (padding * 2);

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
    int rem = displayIndex / width;
    int inputRow = textStartLine + rem;
    int inputCol = displayIndex % width;

    // Move to stats line
    terminalManager.hideCursor();
    terminalManager.moveCursor(statsRow, 0);
    terminalManager.writeToTerminal((char *)"\033[2K", 4); // Clear entire line
    spdlog::info("Moved Cursor to position: {} 0", statsRow);
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

  uiWidget header(width, height, terminalManager);
  header.drawBox(2, 1, 30, 10, headerTitle, true, borderShape::SHARP_SINGLE,
                 (char *)WHITE, true);
  // drawHeader("Terminal Typing Test");
  spdlog::info("Header drawing complete");
  // drawEmptyLine();
  // spdlog::info("Empty line drawing complete");
  // drawStats(0, 0, 0);
  // spdlog::info("Stats line drawing complete");

  // terminalManager.moveCursor(textStartLine, 0);

  // spdlog::info("Moved cursor to position {} 0", textStartLine);
  // terminalManager.writeToTerminal((char *)WHITE, strlen(WHITE));
  // terminalManager.writeToTerminal((char *)state.targetSequence.data(),
  //                                 state.targetSequence.size());
  // terminalManager.writeToTerminal((char *)CRESET, strlen(CRESET));
  //
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

  int rem = displayIndex / width;
  int row = textStartLine + rem;
  int col = displayIndex % width;
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
