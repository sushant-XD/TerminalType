#include "ui/screens/canvas.h"

Canvas::Canvas(terminalCtrl &term)
    : terminal(term), mainBox(terminal.getTerminalWidth(),
                              terminal.getTerminalHeight(), terminal),
      canvasWidth((term.getTerminalWidth() > 60) ? term.getTerminalWidth() - 20
                                                 : term.getTerminalWidth()),
      canvasHeight((term.getTerminalHeight() > 20)
                       ? term.getTerminalHeight() - 5
                       : term.getTerminalHeight()),
      canvasX((term.getTerminalWidth() - canvasWidth) / 2),
      canvasY((term.getTerminalHeight() - canvasHeight) / 2),
      mainLayout(canvasX, canvasY, canvasWidth, canvasHeight) {
  isRendered = false;

  spdlog::info("Main Box Dimensions:({},{}) {}x{}", mainLayout.main.x,
               mainLayout.main.y, mainLayout.main.width,
               mainLayout.main.height);
}

Canvas::~Canvas() {}

void Canvas::drawCanvas() {
  if (isRendered)
    return;
  mainBox.drawBox(mainLayout.main, true, borderShape::SHARP_SINGLE,
                  (char *)WHITE, true);
  isRendered = true;
}
