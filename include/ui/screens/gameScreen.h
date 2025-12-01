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

  std::vector<std::string> wrapText(State &state);

public:
  gameScreen(terminalCtrl &terminal);
  ~gameScreen();

  void render(const State &state) override;
  void update(const State &state) override;
  void clear() override;

  void updateStats(State &state);
};
