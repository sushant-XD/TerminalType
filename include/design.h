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
  std::string formatResultsContent(state_t &state);

private:
  uiWidget mainScreen;
  uiWidget header;
  uiWidget stats;
  uiWidget mainTextBox;

  uiWidget resultsContent;
  uiWidget resultsHeader;

  uiWidget footerContent;

  int terminalHeight;
  int terminalWidth;

  int windowWidth;
  int windowHeight;
  int windowStartRow;
  int windowStartCol;

  int headerStartRow;
  int StatsStartRow;
  int displayTextStartRow;
  int resultsStartRow;

  terminalCtrl &terminalManager;

  std::vector<std::string> wrapText(state_t &state);
};
