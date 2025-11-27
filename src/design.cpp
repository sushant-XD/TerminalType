#include "design.h"
#include <spdlog/spdlog.h>

#define BORDER_COLOR BCYAN
#define DIVIDER_COLOR BBLACK
#define TEXT_COLOR WHITE
#define INPUT_TEXT_COLOR WHITE
#define ERROR_TEXT_COLOR RED
#define CORRECT_TEXT_COLOR GREEN

using namespace std::chrono;

screenState::screenState(terminalCtrl &terminalManager)
    : terminalManager(terminalManager),
      mainScreen(terminalManager.getTerminalWidth(),
                 terminalManager.getTerminalHeight(), terminalManager),
      header(terminalManager.getTerminalWidth(),
             terminalManager.getTerminalHeight(), terminalManager),
      stats(terminalManager.getTerminalWidth(),
            terminalManager.getTerminalHeight(), terminalManager),
      mainTextBox(terminalManager.getTerminalWidth(),
                  terminalManager.getTerminalHeight(), terminalManager),
      resultsContent(terminalManager.getTerminalWidth(),
                     terminalManager.getTerminalHeight(), terminalManager),
      resultsHeader(terminalManager.getTerminalWidth(),
                    terminalManager.getTerminalHeight(), terminalManager),
      resultsOptions(terminalManager.getTerminalWidth(),
                     terminalManager.getTerminalHeight(), terminalManager),
      menuUserSettings(terminalManager.getTerminalWidth(),
                       terminalManager.getTerminalHeight(), terminalManager),
      menuOptions(terminalManager.getTerminalWidth(),
                  terminalManager.getTerminalHeight(), terminalManager) {

  clearTerminal();
  terminalWidth = terminalManager.getTerminalWidth();
  terminalHeight = terminalManager.getTerminalHeight();

  windowWidth = terminalWidth - 20;
  windowHeight = terminalHeight - 5;

  windowStartRow = 1;
  windowStartCol = 1;

  headerStartRow = windowStartRow + 1;
  StatsStartRow = headerStartRow + 3;
  displayTextStartRow = StatsStartRow + 3;
  resultsStartRow = headerStartRow + 3;

  menuOptionsStartRow = headerStartRow + 3;
  menuUserSettingsStartRow = headerStartRow + 3;

  menuRendered = false;
  screenCleared = false;

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
  auto elapsed = duration_cast<seconds>(steady_clock::now() - state.startTime);
  float timeInMinutes = elapsed.count() / 60.0f;
  int wpm = timeInMinutes > 0 ? (state.correctCount / 5) / timeInMinutes : 0;

  std::string statsContent =
      "Time Remaining: " + std::to_string(state.remainingTimeSeconds) +
      "    |    WPM: " + std::to_string(wpm);
  int StatsStartRow = stats.getTextStartRow();
  int StatsStartCol = stats.getTextStartColumn();
  stats.updateText((char *)statsContent.c_str(), StatsStartRow, StatsStartCol,
                   statsContent.size(), (char *)WHITE);
}

// Complete gradient box render function
void screenState::renderStartScreen(state_t &state) {
  if (!screenCleared) {
    clearTerminal();
    screenCleared = true;
  }
  spdlog::info("terminal cleared");

  std::string headerTitle = "Terminal Typing Test";
  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";

  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  mainScreen.drawBox(windowStartCol, windowStartRow, windowWidth, windowHeight,
                     true, borderShape::SHARP_SINGLE, (char *)BLUE, true);
  header.drawBoxWithText(windowStartCol + 1, headerStartRow, windowWidth - 2, 3,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);
  stats.drawBoxWithText(windowStartCol + 1, StatsStartRow, windowWidth - 2, 3,
                        statsContent, true, borderShape::SHARP_SINGLE,
                        (char *)WHITE, (char *)WHITE, false);
  mainTextBox.drawBoxWithText(
      windowStartCol + 1, displayTextStartRow, windowWidth - 2, 6, displayText,
      true, borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);
  spdlog::info("Render Gradient Setup complete");
}

// Complete gradient box render function
void screenState::renderMenuScreen(state_t &state) {
  if (menuRendered) {
    return;
  }
  clearTerminal();
  spdlog::info("terminal cleared");

  std::string headerTitle = "Terminal Typing Test";
  std::string statsContent =
      "Time: " + std::to_string(state.totalTimeSeconds) + "    |    Level: 0";
  std::string displayText(state.targetSequence.begin(),
                          state.targetSequence.end());

  mainScreen.drawBox(windowStartCol, windowStartRow, windowWidth, windowHeight,
                     true, borderShape::SHARP_SINGLE, (char *)BLUE, true);
  header.drawBoxWithText(windowStartCol + 1, headerStartRow, windowWidth - 2, 3,
                         headerTitle, true, borderShape::SHARP_SINGLE,
                         (char *)WHITE, (char *)WHITE, true);
  menuOptions.drawBoxWithText(windowStartCol + 1, menuOptionsStartRow,
                              windowWidth - 2, 5, getMenuString(0), false,
                              borderShape::SHARP_SINGLE, (char *)WHITE,
                              (char *)WHITE, false);
  spdlog::info("Moving cursor to position: {} {}", menuOptionsStartRow, 3);
  terminalManager.moveCursor(2, 2);

  int currentRow = 0, currentCol = 0;
  terminalManager.getCurrentCursorPosition(currentRow, currentCol);
  spdlog::info("Current Cursor Position: {} {}", currentRow, currentCol);
  menuRendered = true;
}

selectedMenuOption screenState::updateMenuScreen() {
  int currentRow = 0, currentCol = 0;

  terminalManager.getCurrentCursorPosition(currentRow, currentCol);
  spdlog::info("Current Cursor Position: {} {}", currentRow, currentCol);

  int menuFirstOptRow = menuOptions.getTextStartRow();
  int menuFirstOptCol = 3;
  int selectedOption = currentRow % 3;

  spdlog::info("Selected Option: {} CurrentRow: {} menuFirst Option Row: {}",
               selectedOption, currentRow, menuFirstOptRow);
  menuOptions.erase();
  menuOptions.drawBoxWithText(windowStartCol + 1, menuOptionsStartRow,
                              windowWidth - 2, 5, getMenuString(selectedOption),
                              false, borderShape::SHARP_SINGLE, (char *)WHITE,
                              (char *)WHITE, false);
  terminalManager.moveCursor(menuFirstOptRow + selectedOption, 0);
  if (selectedOption == 0) {
    return selectedMenuOption::START;
  } else if (selectedOption == 1) {
    return selectedMenuOption::SETTINGS;
  } else {
    return selectedMenuOption::QUIT;
  }
}

void screenState::get_and_print_result(state_t &state) {
  clearTerminal();

  // Calculate stats
  auto elapsed = duration_cast<seconds>(steady_clock::now() - state.startTime);
  float timeInMinutes = elapsed.count() / 60.0f;
  int wpm = timeInMinutes > 0 ? (state.correctCount / 5) / timeInMinutes : 0;

  int totalChars = state.correctCount + state.incorrectCount;
  float accuracy =
      totalChars > 0
          ? (static_cast<float>(state.correctCount) / totalChars * 100.0f)
          : 0.0f;

  int minutes = elapsed.count() / 60;
  int seconds = elapsed.count() % 60;

  // Build results content string
  std::string resultsText = "\nWords Per Minute: " + std::to_string(wpm) +
                            "\n WPM    "
                            "\nAccuracy: " +
                            std::to_string((int)accuracy) +
                            "%    "
                            "\nTime: " +
                            std::to_string(minutes) + "m " +
                            std::to_string(seconds) +
                            "s    "
                            "\nTotal Chars: " +
                            std::to_string(totalChars) +
                            "    "
                            "\nCorrect: " +
                            std::to_string(state.correctCount) +
                            "    "
                            "Incorrect: " +
                            std::to_string(state.incorrectCount) +
                            "    "
                            "\nLevel: Easy\n";

  std::string resultsHeaderTitle = "*** TEST COMPLETE ***";

  // Draw using widget system
  mainScreen.drawBox(windowStartCol, windowStartRow, windowWidth, windowHeight,
                     true, borderShape::SHARP_SINGLE, (char *)BLUE, true);

  resultsHeader.drawBoxWithText(windowStartCol + 1, headerStartRow,
                                windowWidth - 2, 3, resultsHeaderTitle, true,
                                borderShape::SHARP_SINGLE, (char *)GREEN,
                                (char *)GREEN, true);

  resultsContent.drawBoxWithText(
      windowStartCol + 1, resultsStartRow, windowWidth - 2, 10, resultsText,
      false, // left align
      borderShape::SHARP_SINGLE, (char *)WHITE, (char *)WHITE, false);

  spdlog::info("Results screen rendered successfully");
}

// void screenState::appendToBuffer(std::vector<char> &buffer, const char *data)
// {
//   if (data) {
//     size_t len = strlen(data);
//     if (len > 0) {
//       buffer.insert(buffer.end(), data, data + len);
//     }
//   }
// }
//
// void screenState::appendToBuffer(std::vector<char> &buffer,
//                                  const std::string &data) {
//   if (!data.empty()) {
//     buffer.insert(buffer.end(), data.begin(), data.end());
//   }
// }
