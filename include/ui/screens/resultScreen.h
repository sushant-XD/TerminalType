#pragma once
#include "ui/screens/canvas.h"
#include "ui/widgets/uiWidgets.h"
#include <spdlog/spdlog.h>

struct ResultLayout {
  Box header;
  Box stats;
  Box options;

  ResultLayout(int canvasX, int canvasY, int canvasWidth, int canvasHeight,
               int numOfOptions) {
    header = {
        .x = canvasX + 1,
        .y = canvasY + 1,
        .width = canvasWidth - 2,
        .height = 3,
    };
    stats = {
        .x = canvasX + 1,
        .y = header.y + header.height + 1,
        .width = canvasWidth - 2,
        .height = 8,
    };
    options = {
        .x = canvasX + 1,
        .y = stats.y + stats.height + 1,
        .width = canvasWidth - 2,
        .height = numOfOptions + 2,
    };
  };
};

class resultScreen : public Canvas {
private:
  uiWidget header;
  uiWidget statsBox;
  uiWidget optionsBox;

  ResultLayout layout;

  bool isRendered;
  ResultOpts currentSelected;
  std::vector<std::string> optionsList;
  int optionsNum;

public:
  resultScreen(terminalCtrl &terminal);
  ~resultScreen();

  void render(State &state);
  void update(const State &state);
  void clear();
  ResultOpts updateSelection(bool up);
  ResultOpts getCurrentSelection() const { return currentSelected; }
};
