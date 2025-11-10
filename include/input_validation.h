#pragma once

#include "configurations.h"
#include "terminal_ctrl.h"

void setup();
class inputValidator {
public:
  inputValidator(terminalCtrl &terminalManager);
  ~inputValidator();
  int getInputAndCompare(state_t &state, char ch);

private:
  result_t res;
  terminalCtrl &terminalManager;
};
