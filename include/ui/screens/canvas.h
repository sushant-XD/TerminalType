#pragma once
#include "ui/widgets/uiWidgets.h"
#include <string>
#include <vector>

class Canvas {
public:
  Canvas(terminalCtrl &terminal);
  ~Canvas();

  void drawCanvas();

  virtual void render(const state_t &state) = 0;
  virtual void update(const state_t &state) = 0;
  virtual void clear() { terminal.writeToTerminal((char *)CLS, strlen(CLS)); }

protected:
  terminalCtrl &terminal;
  int canvasX;      // initial position in X
  int canvasY;      // initial position in Y
  int canvasWidth;  // canvas Width
  int canvasHeight; // canvas Height

private:
  uiWidget mainBox;
  bool isRendered;
};
