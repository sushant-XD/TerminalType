#pragma once
#include "configurations.h"

extern "C" {
#include "termios.h"
#include "unistd.h"
}
#include "ansi_color_codes.h"
#include <cstring>
#include <sys/ioctl.h>

static constexpr std::string_view SHOW_CURSOR = "\033[?25h";
static constexpr std::string_view HIDE_CURSOR = "\033[?25l";
static constexpr std::string_view SAVE_CURSOR = "\033[s";
static constexpr std::string_view RESTORE_CURSOR = "\033[u";

class terminalCtrl {
public:
  terminalCtrl();
  ~terminalCtrl();
  void setTerminalNonBlocking();
  void resetTerminal();
  int getTerminalWidth();
  int getTerminalHeight();
  char getCharacter();
  void writeToTerminal(char *ch, int size);
  void getCurrentCursorPosition(int &row, int &col);

  void moveCursor(int row, int col);

  void saveCursor();
  void restoreCursor();
  void showCursor();
  void hideCursor();

  char *getAllCharacters();

private:
  int bufSize = 1;
  int input_fd;  // used to set terminal characteristics
  int output_fd; // used to get terminal size and stuff
  struct termios terminal_settings;
  struct termios terminal_original;
};
