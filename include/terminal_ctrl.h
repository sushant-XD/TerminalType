#pragma once
#include "configurations.h"

extern "C" {
#include "termios.h"
#include "unistd.h"
}
#include "ansi_color_codes.h"
#include <cstring>
#include <sys/ioctl.h>

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

private:
  int bufSize = 1;
  int input_fd;  // used to set terminal characteristics
  int output_fd; // used to get terminal size and stuff
  struct termios terminal_settings;
  struct termios terminal_original;
};
