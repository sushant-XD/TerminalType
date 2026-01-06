#include "gameEngine/terminal_ctrl.h"
#include <cstdio>
#include <termios.h>

terminalCtrl::terminalCtrl() {
  input_fd = STDIN_FILENO;
  output_fd = STDOUT_FILENO;
  tcgetattr(input_fd, &terminal_original);
  terminal_settings = terminal_original;
  setTerminalNonBlocking();

  // Enter alternate screen buffer
  writeToTerminal(const_cast<char *>(ENTER_ALT_SCREEN.data()),
                  ENTER_ALT_SCREEN.size());
  hideCursor(); // You probably want this too
}

terminalCtrl::~terminalCtrl() {
  showCursor();
  // Exit alternate screen buffer - restores original terminal content
  writeToTerminal(const_cast<char *>(EXIT_ALT_SCREEN.data()),
                  EXIT_ALT_SCREEN.size());
  resetTerminal();
  memset(&terminal_settings, '\0', sizeof(terminal_settings));
}

void terminalCtrl::setTerminalNonBlocking() {
  terminal_settings.c_lflag &= ~ICANON; // enable non-canonical mode
  terminal_settings.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
  terminal_settings.c_cc[VMIN] = 0;
  terminal_settings.c_cc[VTIME] = 0;
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
  size_t bytesRead = read(input_fd, tmpBuf, bufSize);
  if (bytesRead > 0) {
    return tmpBuf[0];
  }
  return '\0';
}

char *terminalCtrl::getAllCharacters() {
  char tmpBuf[1024];
  size_t bytesRead = read(input_fd, tmpBuf, sizeof(tmpBuf));
  if (bytesRead > 0) {
    return tmpBuf;
  }
  return nullptr;
}

void terminalCtrl::writeToTerminal(char *ch, int size) {
  write(output_fd, ch, size);
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

void terminalCtrl::getCurrentCursorPosition(int &row, int &col) {
  // Write to stdout, not stdin
  const char inp[] = "\033[6n";
  write(output_fd, inp, sizeof(inp) - 1); // -1 to exclude null terminator

  // Small delay to allow terminal to respond
  usleep(10000); // 10ms delay

  char buffer[32];
  memset(buffer, 0, sizeof(buffer));

  int bytes_read = read(input_fd, buffer, sizeof(buffer) - 1);

  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    // Parse the response: ESC[row;colR
    if (sscanf(buffer, "\033[%d;%dR", &row, &col) != 2) {
      row = 0;
      col = 0;
    }
  } else {
    row = 0;
    col = 0;
  }
}

void terminalCtrl::clearTerminal() {
  writeToTerminal((char *)CLS, strlen(CLS));
}

void terminalCtrl::moveCursor(int row, int col) {
  char buffer[32];
  int len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", row, col);
  writeToTerminal(buffer, len);
}

void terminalCtrl::moveCursorDown() {
  char buffer[32];
  int len = snprintf(buffer, sizeof(buffer), "\033[1B");
  writeToTerminal(buffer, len);
}

void terminalCtrl::moveCursorUp() {
  char buffer[32];
  int len = snprintf(buffer, sizeof(buffer), "\033[1A");
  writeToTerminal(buffer, len);
}

void terminalCtrl::saveCursor() {
  writeToTerminal(const_cast<char *>(SAVE_CURSOR.data()), SAVE_CURSOR.size());
}

void terminalCtrl::restoreCursor() {
  writeToTerminal(const_cast<char *>(RESTORE_CURSOR.data()),
                  RESTORE_CURSOR.size());
}

void terminalCtrl::showCursor() {
  writeToTerminal(const_cast<char *>(SHOW_CURSOR.data()), SHOW_CURSOR.size());
}

void terminalCtrl::hideCursor() {
  writeToTerminal(const_cast<char *>(HIDE_CURSOR.data()), HIDE_CURSOR.size());
}
