#include "design.h"
#include <sstream>

#define BORDER_COLOR BCYAN
#define DIVIDER_COLOR BBLACK
#define TEXT_COLOR WHITE
#define INPUT_TEXT_COLOR WHITE
#define ERROR_TEXT_COLOR RED
#define CORRECT_TEXT_COLOR GREEN

screenState::screenState(terminalCtrl &terminalManager)
    : terminalManager(terminalManager), initialSetupComplete(false) {
  clearTerminal();
  height = terminalManager.getTerminalHeight();
  width = terminalManager.getTerminalWidth();
  outerCommonPaddingLR = 4;
  inputBoxPaddingLR = 3;
  linSpacing = 1;
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

// Complete gradient box render function
void screenState::renderGradientBox(std::vector<char> &text, float progress,
                                    int wpm, float accuracy, int errors,
                                    int timeRemaining) {
  if (!initialSetupComplete) {
    clearTerminal();
    drawHeader("Terminal Typing Test");
    drawEmptyLine();
  }
  drawStats(timeRemaining, wpm, 0);

  for (int i = 0; i < text.size(); i++) {
    std::cout << text[i];
  }
  std::cout << std::endl;
}

void screenState::renderTextProgress(std::vector<char> &targetText,
                                     std::vector<char> &typedText) {
  int col = typedText.size() - 1;
  moveCursor(6, col);
  std::cout << "\r"; // return to line start (overwrite)
  for (size_t i = 0; i < targetText.size(); ++i) {
    if (i < typedText.size()) {
      if (typedText[i] == targetText[i]) {
        std::cout << GREEN << targetText[i] << CRESET;
      } else {
        std::cout << RED << targetText[i] << CRESET;
      }
    } else {
      std::cout << WHITE << targetText[i] << CRESET;
    }
  }
  // std::cout.flush();
  moveCursor((targetText.size() + 5) / height, 1);
}
