#include "input_validation.h"

inputValidator::inputValidator(terminalCtrl &terminalManager)
    : terminalManager(terminalManager), res() {}

inputValidator::~inputValidator() {}

/*TODO: Fix a few things after rudimentary implementation
 * 1) Backspace clicked when correct character was pressed shouldn't count as
 * wrong 2) Backspace clicked when incorrect chracter was pressed should count
 * as wrong
 * */
int inputValidator::getInputAndCompare(state_t &state, char ch) {
  if (ch == '\b' || ch == 127) {
    if (!state.userInputSequence.empty()) {
      state.userInputSequence.pop_back();

      if (state.incorrectCount > 0) {
        state.incorrectCount--;
      } else if (state.correctCount > 0) {
        state.correctCount--;
      }

      if (state.charCount > 0)
        state.charCount--;

      state.currentKeyStatus = keyStroke::BACKSPACE;
    }
    return 0;
  }

  state.userInputSequence.push_back(ch);
  if (state.incorrectCount > 0) {
    state.incorrectCount++;
    state.currentKeyStatus = keyStroke::INCORRECT;
  } else {
    if (ch == state.targetSequence[state.charCount]) {
      state.correctCount++;
      state.currentKeyStatus = keyStroke::CORRECT;
    } else {
      state.incorrectCount++;
      state.currentKeyStatus = keyStroke::INCORRECT;
    }
  }
  state.charCount++;

  if (ch == ' ' && state.incorrectCount == 0) {
    state.userInputSequence.clear();
  }

  return 0;
}

result_t inputValidator::getResult(int time) {
  res.result_id = 0;
  // res.timeTaken = time;
  // res.level = EASY;
  // res.correct_characters = correctCount;
  // res.total_characters = characterCount;
  // if (res.total_characters == 0) {
  //   res.accuracy = 0;
  // } else {
  //   res.accuracy = 100 * (static_cast<float>((res.correct_characters) * 1.0 /
  //                                            res.total_characters));
  // }
  return res;
}

void inputValidator::print_result() {
  // std::cout << "Time Taken:          " << res.timeTaken << std::endl;
  // std::cout << "Level:               " << res.level << std::endl;
  // std::cout << "Correct Characters:  " << res.correct_characters << "/"
  //           << res.total_characters << std::endl;
  // std::cout << "Accuracy:            " << res.accuracy << "%" << std::endl;
}
