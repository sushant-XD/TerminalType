#pragma once
#include "ui/screens/canvas.h"

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

  void render(const state_t &state) override;
  void update(const state_t &state) override;
  void clear() override;

  void updateStats(state_t &state);
};
