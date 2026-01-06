#pragma once
#include "ui/screens/canvas.h"

struct GameLayout {
  Box stats;
  Box textBox;

  GameLayout(int canvasX, int canvasY, int canvasWidth, int canvasHeight) {
    stats = {
        .x = canvasX + 1,
        .y = canvasY + 1,
        .width = canvasWidth - 2,
        .height = 3,
    };
    textBox = {
        .x = canvasX + 1,
        .y = stats.y + stats.height + 1,
        .width = canvasWidth - 2,
        .height = 12,
    };
  };
};

class gameScreen : public Canvas {
private:
  bool isRendered;

  uiWidget stats;
  uiWidget mainTextBox;

  GameLayout layout;

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
