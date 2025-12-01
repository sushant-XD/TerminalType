#include "ui/screens/canvas.h"

Canvas::Canvas(terminalCtrl &term)
    : terminal(term), mainBox(terminal.getTerminalWidth(),
                              terminal.getTerminalHeight(), terminal) {
  canvasWidth = (terminal.getTerminalWidth() > 60)
                    ? terminal.getTerminalWidth() - 20
                    : terminal.getTerminalWidth();
  canvasHeight = (terminal.getTerminalHeight() > 20)
                     ? terminal.getTerminalHeight() - 5
                     : terminal.getTerminalHeight();

  canvasX = (terminal.getTerminalWidth() - canvasWidth) / 2;

  canvasY = (terminal.getTerminalHeight() - canvasHeight) / 2;

  isRendered = false;
  spdlog::info("Terminal WIdth and Height: {},{}", terminal.getTerminalWidth(),
               terminal.getTerminalHeight());
  spdlog::info("Created Canvas Successfully. Canvas Dimensions: {}X{} Canvas "
               "Initial Position: ({},{})",
               canvasWidth, canvasHeight, canvasX, canvasY);
}

Canvas::~Canvas() {}

void Canvas::drawCanvas() {
  if (isRendered)
    return;
  mainBox.drawBox(canvasX, canvasY, canvasWidth, canvasHeight, true,
                  borderShape::SHARP_SINGLE, (char *)WHITE, true);
  isRendered = true;
}
