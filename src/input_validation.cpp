#include "input_validation.h"
#include <spdlog/spdlog.h>

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
    spdlog::info("Back Key Pressed");
    if (!state.userInputSequence.empty()) {
      state.userInputSequence.pop_back();

      if (state.incorrectCount > 0) {
        state.incorrectCount--;
        spdlog::info("IncorrectCount decreased to: {}", state.incorrectCount);
      } else if (state.correctCount > 0) {
        state.correctCount--;
        spdlog::info("CorrectCount decreased to: {}", state.correctCount);
      }

      if (state.charCount > 0) {
        state.charCount--;
        spdlog::info("Character Count decreased to: {}", state.charCount);
      }

      state.currentKeyStatus = keyStroke::BACKSPACE;
    }
    return 0;
  }

  state.userInputSequence.push_back(ch);
  if (state.incorrectCount > 0) {
    state.incorrectCount++;
    spdlog::info("{} key incorrect. Incorrect Count: {}", ch,
                 state.incorrectCount);
    state.currentKeyStatus = keyStroke::INCORRECT;
  } else {
    if (ch == state.targetSequence[state.charCount]) {
      state.correctCount++;

      spdlog::info("{} key Correct. Correct Count: {}", ch, state.correctCount);
      state.currentKeyStatus = keyStroke::CORRECT;
    } else {
      state.incorrectCount++;
      spdlog::info("{} key Incorrect. Does not match: {}. Incorrect Count: {}",
                   ch, state.targetSequence[state.charCount],
                   state.incorrectCount);
      state.currentKeyStatus = keyStroke::INCORRECT;
    }
  }
  state.charCount++;
  spdlog::info("Character Count: {}", state.charCount);
  if (ch == ' ' && state.incorrectCount == 0) {
    state.userInputSequence.clear();
    spdlog::info("Cleared userInputSequence. Correct word detected.");
  }

  return 0;
}
