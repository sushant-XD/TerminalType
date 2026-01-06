#pragma once

#include "ui/screens/canvas.h"

struct MenuLayout {

  Box header;
  Box options;

  MenuLayout(int canvasX, int canvasY, int canvasWidth, int canvasHeight,
             int numOfOptions) {
    header = {.x = canvasX + 1,
              .y = canvasY + 1,
              .width = canvasWidth - 2,
              .height = 3};

    options = {.x = canvasX + 1,
               .y = header.y + header.height,
               .width = canvasWidth - 2,
               .height = numOfOptions + 2};
  }
};

class menuScreen : public Canvas {
private:
  bool isRendered;
  MenuOpts currentSelected;

  std::vector<std::string> options;

  MenuLayout layout;

  uiWidget header;
  uiWidget menuOptions;

  void drawMenuOptions();

  std::string formatOptionsWithHighlight(const std::vector<std::string> &opts,
                                         int selectedIndex);

public:
  menuScreen(terminalCtrl &terminal);

  void render(State &state) override;
  void update(const State &state) override;
  void clear() override;
  MenuOpts updateSelection(bool up);
};
