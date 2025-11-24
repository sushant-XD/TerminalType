#pragma once
#include "configurations.h"
#include "terminal_ctrl.h"

static constexpr int BACKSPACE_KEY = 127;
static constexpr int BACK_WORD_KEY = 23;

class inputValidator {
public:
  inputValidator(terminalCtrl &terminalManager);
  ~inputValidator();
  int getInputAndCompare(state_t &state, char ch);

private:
  result_t res;
  terminalCtrl &terminalManager;
};
