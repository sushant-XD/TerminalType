#pragma once
#include "ui/screens/canvas.h"
#include "ui/widgets/uiWidgets.h"
#include <spdlog/spdlog.h>

class resultScreen : public Canvas {
private:
  uiWidget header;
  uiWidget statsBox;
  uiWidget optionsBox;

  int headerStartX, headerStartY;
  int headerWidth, headerHeight;

  int statsStartX, statsStartY;
  int statsWidth, statsHeight;

  int optionsStartX, optionsStartY;
  int optionsWidth, optionsHeight;

  bool isRendered;
  ResultOpts currentSelected;
  std::vector<std::string> optionsList;
  int optionsNum;

  std::string getOptionsString(int option);

public:
  resultScreen(terminalCtrl &terminal);
  ~resultScreen();

  void render(State &state);
  void update(const State &state);
  void clear();
  ResultOpts updateSelection(bool up);
  ResultOpts getCurrentSelection() const { return currentSelected; }
};
