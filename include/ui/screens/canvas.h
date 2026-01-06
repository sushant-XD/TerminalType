#pragma once
#include "ui/widgets/uiWidgets.h"
#include "utils/commonFunctions.h"
#include <cassert>
#include <string>
#include <vector>

struct CanvasLayout {
  Box main;
  CanvasLayout(int canvasX, int canvasY, int canvasWidth, int canvasHeight) {
    main = {
        .x = canvasX,
        .y = canvasY,
        .width = canvasWidth,
        .height = canvasHeight,
    };
  };
};

class Canvas {
public:
  Canvas(terminalCtrl &terminal);
  ~Canvas();

  void drawCanvas();

  virtual void render(State &state) = 0;
  virtual void update(const State &state) = 0;
  virtual void clear() { terminal.writeToTerminal((char *)CLS, strlen(CLS)); }

protected:
  terminalCtrl &terminal;

  int canvasWidth;  // canvas Width
  int canvasHeight; // canvas Height
  int canvasX;      // initial position in X
  int canvasY;      // initial position in Y
  CanvasLayout mainLayout;

private:
  uiWidget mainBox;
  bool isRendered;
};
