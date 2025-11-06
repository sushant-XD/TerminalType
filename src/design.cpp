#include "design.h"
#include <ostream>

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
  outerCommonPaddingLR = 4;
  inputBoxPaddingLR = 3;
  linSpacing = 1;
  textStartLine = 6;
  statsRow = 5;
}

screenState::~screenState() {}

void screenState::clearTerminal() { std::cout << CLS << std::endl; }

void screenState::drawHeader(std::string content) {
  int contentLength = content.size();
  int remainingLength = (width - contentLength - 2) / 2; // -2 for borders

  // Top line
  std::cout << BORDER_COLOR << BoxChars::TOP_LEFT_ROUND;
  for (int i = 0; i < width - 2; i++) {
    std::cout << BoxChars::HORIZONTAL;
  }
  std::cout << BoxChars::TOP_RIGHT_ROUND << CRESET << std::endl;

  // Content line
  std::cout << BORDER_COLOR << BoxChars::VERTICAL << CRESET
            << std::string(remainingLength, ' ') << BWHITE << content << CRESET
            << std::string(remainingLength, ' ') << BORDER_COLOR
            << BoxChars::VERTICAL << CRESET << std::endl;

  // Divider line
  std::cout << DIVIDER_COLOR << BoxChars::T_LEFT;
  for (int i = 0; i < width - 2; i++) {
    std::cout << BoxChars::HORIZONTAL;
  }
  std::cout << BoxChars::T_RIGHT << CRESET << std::endl;
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

  std::cout << std::string(sideSpace, ' ') << left
            << std::string(innerPadding, ' ') << middle
            << std::string(innerPadding, ' ') << right << std::endl;
}

void screenState::drawEmptyLine() {
  std::cout << BORDER_COLOR << BoxChars::VERTICAL << CRESET
            << std::string(width - 2, ' ') << BORDER_COLOR << BoxChars::VERTICAL
            << CRESET << std::endl;
}

void screenState::updateStats(state_t &state) {
  if (state.isRunning) {
    auto elapsed =
        duration_cast<seconds>(steady_clock::now() - state.startTime);
    float timeInMinutes = elapsed.count() / 60.0f;
    int wpm = timeInMinutes > 0 ? (state.correctCount / 5) / timeInMinutes : 0;

    // Move to stats line
    moveCursor(statsRow, 0);

    // Clear the line
    terminalManager.writeToTerminal((char *)"\033[2K", 4); // Clear entire line

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

    terminalManager.writeToTerminal((char *)statsLine.c_str(),
                                    statsLine.length());
  }
}
// Complete gradient box render function
void screenState::renderGradientBox(state_t &state) {
  clearTerminal();
  drawHeader("Terminal Typing Test");
  drawEmptyLine();
  drawStats(0, 0, 0);

  std::cout << BBLACK;
  for (int i = 0; i < state.targetSequence.size(); i++) {
    std::cout << state.targetSequence[i];
  }
  std::flush(std::cout);
}

void screenState::renderTextProgress(state_t &state) {
  int displayIndex;
  if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    displayIndex = state.charCount;
  } else {
    displayIndex = state.charCount - 1;
  }
  int rem = displayIndex / width;
  int row = textStartLine + rem;
  int col = displayIndex % width;
  moveCursor(row, col);
  if (state.currentKeyStatus == keyStroke::CORRECT) {
    terminalManager.writeToTerminal((char *)GREEN, sizeof(GREEN));
  } else if (state.currentKeyStatus == keyStroke::INCORRECT) {
    terminalManager.writeToTerminal((char *)RED, sizeof(RED));
  } else if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    terminalManager.writeToTerminal((char *)BBLACK, sizeof(BBLACK));
  }
  terminalManager.writeToTerminal((char *)&state.targetSequence[displayIndex],
                                  1);
  terminalManager.writeToTerminal((char *)CRESET, sizeof(CRESET));
  if (state.currentKeyStatus == keyStroke::BACKSPACE) {
    moveCursor(row, col);
  }
}

void screenState::moveCursor(int row, int col) {
  char buffer[32];
  int len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", row + 1, col + 1);
  terminalManager.writeToTerminal(buffer, len);
}
