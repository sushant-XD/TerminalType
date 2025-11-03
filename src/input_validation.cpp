#include "input_validation.h"

inputValidator::inputValidator(std::vector<char> &fileChars,
                               terminalCtrl &terminalManager)
    : terminalManager(terminalManager), fileBuffer(fileChars),
      characterCount(0), correctCount(0) {
  inputBuffer = {};
}

inputValidator::~inputValidator() {
  characterCount = 0;
  correctCount = 0;
  fileBuffer = {};
  inputBuffer = {};
}

/*TODO: Fix a few things after rudimentary implementation
 * 1) Backspace clicked when correct character was pressed shouldn't count as
 * wrong 2) Backspace clicked when incorrect chracter was pressed should count
 * as wrong
 * */
void inputValidator::getInputAndCompare(char ch) {
  if (ch == '\b' || ch == 127) {
    if (inputBuffer.empty()) {
      return;
    }

    inputBuffer.pop_back();
    characterCount--;
    terminalManager.writeToTerminal(const_cast<char *>(BACKSPACE), 3);
    return;
  }
  inputBuffer.push_back(ch);
  terminalManager.writeToTerminal(&ch, 1);

  if (ch == fileBuffer[characterCount]) {
    correctCount++;
  }
  characterCount++;
}

result_t inputValidator::getResult(int time) {
  res.result_id = 0;
  res.timeTaken = time;
  res.level = EASY;
  res.correct_characters = correctCount;
  res.total_characters = characterCount;
  if (res.total_characters == 0) {
    res.accuracy = 0;
  } else {
    res.accuracy = 100 * (static_cast<float>((res.correct_characters) * 1.0 /
                                             res.total_characters));
  }
  return res;
}

void inputValidator::print_result() {
  std::cout << "Time Taken:          " << res.timeTaken << std::endl;
  std::cout << "Level:               " << res.level << std::endl;
  std::cout << "Correct Characters:  " << res.correct_characters << "/"
            << res.total_characters << std::endl;
  std::cout << "Accuracy:            " << res.accuracy << "%" << std::endl;
}
