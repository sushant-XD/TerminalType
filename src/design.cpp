#include "design.h"
#include <sstream>

#define BORDER_COLOR BCYAN
#define DIVIDER_COLOR BBLACK
#define TEXT_COLOR WHITE
#define INPUT_TEXT_COLOR WHITE
#define ERROR_TEXT_COLOR RED
#define CORRECT_TEXT_COLOR GREEN

screenState::screenState(terminalCtrl &terminalManager)
    : terminalManager(terminalManager) {
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

void screenState::drawEmptyLine() {
  std::cout << BORDER_COLOR << BoxChars::VERTICAL << CRESET
            << std::string(width - 2, ' ') << BORDER_COLOR << BoxChars::VERTICAL
            << CRESET << std::endl;
}

void screenState::drawText(std::vector<char> &text) {
  // Calculate usable width for text
  int usableWidth = width - outerCommonPaddingLR * 2 - 2; // -2 for borders

  // Convert vector to string for easier handling
  std::string textStr(text.begin(), text.end());

  // Word wrap the text
  std::vector<std::string> lines = wrapText(textStr, usableWidth);

  // Draw each line
  for (const auto &line : lines) {
    int leftPadding = outerCommonPaddingLR;
    int rightPadding = width - line.size() - leftPadding - 2;

    std::cout << BORDER_COLOR << BoxChars::VERTICAL << CRESET
              << std::string(leftPadding, ' ') << WHITE << line << CRESET
              << std::string(rightPadding, ' ') << BORDER_COLOR
              << BoxChars::VERTICAL << CRESET << std::endl;
  }
}

// Complete gradient box render function
void screenState::renderGradientBox(std::vector<char> &text, float progress,
                                    int wpm, float accuracy, int errors,
                                    int timeRemaining) {
  clearTerminal();
  drawHeader("Terminal Typing Test");
  drawEmptyLine();
}
