#pragma once

#include "canvas.h"

class menuScreen : public Canvas {
private:
  bool isRendered;

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

  std::string getMenuString(int option);

public:
  menuScreen(terminalCtrl &terminal);

  void render(state_t &state) override;
  void update(state_t &state) override;
  void clear() override;
  selectedMenuOption updateSelection();
};
