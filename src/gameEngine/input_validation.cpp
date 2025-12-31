#include "gameEngine/input_validation.h"
#include <spdlog/spdlog.h>

inputValidator::inputValidator(terminalCtrl &terminalManager)
    : terminalManager(terminalManager), res() {}

inputValidator::~inputValidator() {}

int inputValidator::getInputAndCompare(State &state, char ch) {
  // backspace and back word (Ctrl+backspace) handled manually because termios
  // and raw inputs
  if (ch == BACKSPACE_KEY) {
    spdlog::info("Back Key Pressed");
    if (!state.userInputSequence.empty()) {
      state.userInputSequence.pop_back();

      if (state.incorrectCount > 0) {
        state.incorrectCount--;
      } else if (state.correctCount > 0) {
        state.correctCount--;
      }

      if (state.charCount > 0) {
        state.charCount--;
      }
      state.backspaceCount = 1;
      spdlog::debug("Correct Count: {} Incorrect Count: {} Char Count: {}",
                    state.correctCount, state.incorrectCount, state.charCount);
      state.currentKeyStatus = KeyStroke::BACKSPACE;
    } else {
      state.backspaceCount = 0;
    }
    return 0;
  }
  // Ctrl+W
  if (ch == BACK_WORD_KEY) {
    spdlog::info("Back Word Pressed. Before erasing word: {}",
                 std::string(state.userInputSequence.begin(),
                             state.userInputSequence.end()));
    int deletedChars = 0;
    while ((!state.userInputSequence.empty()) &&
           (state.userInputSequence.back() == ' ')) {
      state.userInputSequence.pop_back();
      if (state.incorrectCount > 0) {
        state.incorrectCount--;
      } else if (state.correctCount > 0) {
        state.correctCount--;
      }
      if (state.charCount > 0) {
        state.charCount--;
      }
      deletedChars++;
    }

    while ((!state.userInputSequence.empty()) &&
           (state.userInputSequence.back() != ' ')) {
      state.userInputSequence.pop_back();
      if (state.incorrectCount > 0) {
        state.incorrectCount--;
      } else if (state.correctCount > 0) {
        state.correctCount--;
      }
      if (state.charCount > 0) {
        state.charCount--;
      }
      deletedChars++;
    }
    state.backspaceCount = deletedChars;
    spdlog::debug("After erasing word: {}",
                  std::string(state.userInputSequence.begin(),
                              state.userInputSequence.end()));
    spdlog::debug("Correct Count: {} Incorrect Count: {} Char Count: {}",
                  state.correctCount, state.incorrectCount, state.charCount);
    state.currentKeyStatus = KeyStroke::BACK_WORD;
    return 0;
  }

  state.backspaceCount = 0;
  state.userInputSequence.push_back(ch);
  state.totalPressed++;
  if (state.incorrectCount > 0) {
    state.incorrectCount++;
    spdlog::info("{} key incorrect. Incorrect Count: {}", ch,
                 state.incorrectCount);
    state.currentKeyStatus = KeyStroke::INCORRECT;
  } else {
    if (ch == state.targetSequence[state.charCount]) {
      state.correctCount++;
      state.totalCorrect++;
      spdlog::info("{} key Correct. Correct Count: {}", ch, state.correctCount);
      state.currentKeyStatus = KeyStroke::CORRECT;
    } else {
      state.incorrectCount++;
      spdlog::info("{} key Incorrect. Does not match: {}. Incorrect Count: {}",
                   ch, state.targetSequence[state.charCount],
                   state.incorrectCount);
      state.currentKeyStatus = KeyStroke::INCORRECT;
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
