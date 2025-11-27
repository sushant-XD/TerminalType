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
  void renderStartScreen(state_t &state);
  void renderTextProgress(state_t &state);
  void testComplete();
  void setTerminalBackground();
  void get_and_print_result(state_t &state);

  void appendToBuffer(std::vector<char> &buffer, const char *data);
  void appendToBuffer(std::vector<char> &buffer, const std::string &data);
  std::string formatResultsContent(state_t &state);

private:
  uiWidget mainScreen;  // main screen with the outer box
  uiWidget header;      // header "Terminal Type Test"
  uiWidget stats;       // stats during test
  uiWidget mainTextBox; // main text box

  uiWidget resultsContent; // results content after the game is over
  uiWidget resultsHeader;  // results header

  uiWidget resultsOptions; // options: Play again, go to home

  uiWidget menuUserSettings;
  uiWidget menuOptions;

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

  int menuUserSettingsStartRow;
  int menuOptionsStartRow;

  bool menuRendered;
  bool screenCleared;

  terminalCtrl &terminalManager;
};
