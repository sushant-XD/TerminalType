#pragma once

#include "ui/screens/canvas.h"

class menuScreen : public Canvas {
private:
  bool isRendered;
  selectedMenuOption currentSelected;

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

  void render(const state_t &state) override;
  void update(const state_t &state) override;
  void clear() override;
  selectedMenuOption updateSelection(bool up);
};
