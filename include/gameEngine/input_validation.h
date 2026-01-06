#pragma once
#include "gameEngine/terminal_ctrl.h"
#include "utils/configurations.h"

static constexpr int BACKSPACE_KEY = 127;
static constexpr int BACK_WORD_KEY = 23;

class inputValidator {
public:
  inputValidator(terminalCtrl &terminalManager);
  ~inputValidator();
  int getInputAndCompare(State &state, char ch);

  int get_results(State &state, int elapsed_time_seconds);

private:
  Result res;
  terminalCtrl &terminalManager;
};
