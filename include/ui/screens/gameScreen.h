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
  std::pair<int, int> calculateCharacterPosition(int charIndex,
                                                 const State &state);
  void updateCharacterColor(int charIndex, const State &state, char *color);

public:
  gameScreen(terminalCtrl &terminal);
  ~gameScreen();

  void render(State &state) override;
  void update(const State &state) override;
  void clear() override;

  void updateStats(State &state);
};
