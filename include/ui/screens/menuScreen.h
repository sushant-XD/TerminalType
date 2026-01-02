#pragma once

#include "ui/screens/canvas.h"

class menuScreen : public Canvas {
private:
  bool isRendered;
  MenuOpts currentSelected;

  std::vector<std::string> menuOptionList;
  int menuOptionsNum;

  int headerStartX;
  int headerStartY;

  int headerWidth;
  int headerHeight;

  int menuOptionsStartX;
  int menuOptionsStartY;

  int menuOptionsWidth;
  int menuOptionsHeight;

  uiWidget header;
  uiWidget menuOptions;

public:
  menuScreen(terminalCtrl &terminal);

  void render(State &state) override;
  void update(const State &state) override;
  void clear() override;
  MenuOpts updateSelection(bool up);
};
