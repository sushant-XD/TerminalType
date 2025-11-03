#include "terminal_ctrl.h"
#include <termios.h>

terminalCtrl::terminalCtrl() {
  input_fd = STDIN_FILENO;
  output_fd = STDOUT_FILENO;
  tcgetattr(input_fd, &terminal_original); // saving original settings
  terminal_settings =
      terminal_original; // set our current settings to original to not change
                         // flags that don't need to change
  setTerminalNonBlocking();
}

terminalCtrl::~terminalCtrl() {
  resetTerminal();
  memset(&terminal_settings, '\0', sizeof(terminal_settings));
}

void terminalCtrl::setTerminalNonBlocking() {
  terminal_settings.c_lflag &= ~ICANON; // enable non-canonical mode
  terminal_settings.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
  if (tcsetattr(input_fd, TCSANOW, &terminal_settings)) {
    // if setattr failed
    resetTerminal();
    terminal_settings = terminal_original;
    return;
  }
  // do nothing if succeeds
}

// get character (non-canonical mode so need to be aware with special
// characters)
char terminalCtrl::getCharacter() {
  char tmpBuf[bufSize];
  read(input_fd, tmpBuf, bufSize);
  return tmpBuf[0];
}

void terminalCtrl::writeToTerminal(char *ch, int size) {
  write(input_fd, ch, size);
}

void terminalCtrl::resetTerminal() {
  tcsetattr(input_fd, TCSANOW, &terminal_original);
}

int terminalCtrl::getTerminalWidth() {
  struct winsize w;
  ioctl(output_fd, TIOCGWINSZ, &w);
  return w.ws_col ? w.ws_col : 80; // fallback to 80 if not available
}

int terminalCtrl::getTerminalHeight() {
  struct winsize ws;
  ioctl(output_fd, TIOCGWINSZ, &ws);
  return ws.ws_row ? ws.ws_row : 20; // set default to 20
}
