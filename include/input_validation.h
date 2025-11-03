#pragma once
#include "configurations.h"
#include "terminal_ctrl.h"

void setup();
class inputValidator {
public:
  inputValidator(std::vector<char> &fileChars, terminalCtrl &terminalManager);
  ~inputValidator();
  void getInputAndCompare(char ch);
  int getCharacterCount() { return characterCount; }
  int getCorrectCharacterCount() { return correctCount; }

  result_t getResult(int time);
  void print_result();

private:
  std::vector<char> fileBuffer;
  std::vector<char> inputBuffer;
  result_t res = {};
  terminalCtrl &terminalManager;
  int characterCount;
  int correctCount;
};
