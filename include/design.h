#pragma once

#include "ansi_color_codes.h"
#include "box_chars.h"
#include "configurations.h"
#include "terminal_ctrl.h"

inline void moveCursor(int row, int col) {
  std::cout << "\033[" << row << ";" << col << "H";
}
class screenState {
public:
  screenState(terminalCtrl &terminalManager);
  ~screenState();

  void clearTerminal();
  void drawStats(int timeRemaining, int speed, int level);
  void drawHeader(std::string content);
  void drawEmptyLine();

  // Main render function
  void renderGradientBox(std::vector<char> &text, float progress, int wpm,
                         float accuracy, int errors, int timeRemaining);
  void renderTextProgress(int charIndex, char ch, bool success);

  void testComplete();

private:
  int height;
  int width;
  terminalCtrl &terminalManager;

  bool initialSetupComplete;

  int outerCommonPaddingLR;
  int linSpacing;
  int inputBoxPaddingLR;
  std::vector<std::string> wrapText(const std::string &text, int maxWidth);
};
