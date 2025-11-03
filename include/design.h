#pragma once

#include "ansi_color_codes.h"
#include "box_chars.h"
#include "configurations.h"
#include "terminal_ctrl.h"

class screenState {
public:
  screenState(terminalCtrl &terminalManager);
  ~screenState();

  void clearTerminal();
  void drawHeader(std::string content);
  void drawEmptyLine();

  // Main render function
  void renderGradientBox(std::vector<char> &text, float progress, int wpm,
                         float accuracy, int errors, int timeRemaining);

private:
  int height;
  int width;
  terminalCtrl &terminalManager;

  int outerCommonPaddingLR;
  int linSpacing;
  int inputBoxPaddingLR;
  std::vector<std::string> wrapText(const std::string &text, int maxWidth);
};
