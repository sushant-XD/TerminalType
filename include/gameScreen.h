#pragma once
#include "canvas.h"
#include "uiWidgets.h"

class gameScreen : public Canvas {
private:
  bool isRendered;

  uiWidget stats;
  uiWidget mainTextBox;

  int statsStartX;
  int statsStartY;

  int statsWidth;
  int statsHeight;

  int mainTextBoxStartX;
  int mainTextBoxStartY;

  int mainTextBoxWidth;
  int mainTextBoxHeight;

  std::vector<std::string> wrapText(state_t &state);

public:
  gameScreen(terminalCtrl &terminal);
  ~gameScreen();

  void render(state_t &state) override;
  void update(state_t &state) override;
  void clear() override;

  void updateStats(state_t &state);
};
