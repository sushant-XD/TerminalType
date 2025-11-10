#pragma once

#include "ansi_color_codes.h"
#include "box_chars.h"
#include "configurations.h"
#include "terminal_ctrl.h"
#include "uiWidgets.h"
#include <chrono>

class screenState {
public:
  screenState(terminalCtrl &terminalManager);
  ~screenState();

  void clearTerminal();
  void drawStats(int timeRemaining, int speed, int level);
  void drawHeader(std::string content);
  void drawEmptyLine();

  // Main render function
  void updateStats(state_t &state);
  void renderGradientBox(state_t &state);
  void renderTextProgress(state_t &state);
  void testComplete();
  void setTerminalBackground();
  void get_and_print_result(state_t &state);

  void appendToBuffer(std::vector<char> &buffer, const char *data);
  void appendToBuffer(std::vector<char> &buffer, const std::string &data);

private:
  void moveCursor(int row, int col);
  int height;
  int statsRow;
  int width;
  int textStartLine;
  terminalCtrl &terminalManager;

  bool initialSetupComplete;

  int outerCommonPaddingLR;
  int linSpacing;
  int inputBoxPaddingLR;
  std::vector<std::string> wrapText(state_t &state);
};
