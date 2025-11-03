#pragma once

#include "configurations.h"
#include "terminal_ctrl.h"

void setup();
class inputValidator {
public:
  inputValidator(std::vector<char> &fileChars, terminalCtrl &terminalManager);
  ~inputValidator();
  int getInputAndCompare(state_t &state, char ch);
  int getCharacterCount() { return characterCount; }
  int getCorrectCharacterCount() { return correctCount; }

  result_t getResult(int time);
  void print_result();

  std::vector<char> inputBuffer;

  int characterCount;

  int lastCorrectCharIndex;

  char currentOriginalCharacter;

private:
  std::vector<char> fileBuffer;
  result_t res = {};
  terminalCtrl &terminalManager;
  int correctCount;
  int invalidCharacterCount;
};
