#include "gameEngine/input_validation.h"
#include <spdlog/spdlog.h>

static constexpr bool wordByWord = true;

inputValidator::inputValidator(terminalCtrl &terminalManager)
    : terminalManager(terminalManager), res() {}

inputValidator::~inputValidator() {}

int inputValidator::getInputAndCompare(State &state, char ch) {
  // backspace and back word (Ctrl+backspace) handled manually because termios
  // and raw inputs
  if (ch == BACKSPACE_KEY) {
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
      state.currentKeyStatus = KeyStroke::BACKSPACE;
    } else {
      state.backspaceCount = 0;
    }
    spdlog::debug("backspace. (CoC: {},ICC: {}, CaC: {}, BC: {}, CS: {})",
                  state.correctCount, state.incorrectCount, state.charCount,
                  state.backspaceCount,
                  state.userInputSequence.empty()
                      ? "_"
                      : std::string(state.userInputSequence.begin(),
                                    state.userInputSequence.end()));
    return 0;
  }
  // Ctrl+W
  if (ch == BACK_WORD_KEY) {
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
    state.currentKeyStatus = KeyStroke::BACK_WORD;
    spdlog::debug("Ctrl+W (CoC: {},ICC: {}, CaC: {}, BC: {}, CS: {})",
                  state.correctCount, state.incorrectCount, state.charCount,
                  state.backspaceCount,
                  state.userInputSequence.empty()
                      ? "_"
                      : std::string(state.userInputSequence.begin(),
                                    state.userInputSequence.end()));
    return 0;
  }

  state.backspaceCount = 0;
  state.userInputSequence.push_back(ch);
  state.totalPressed++;
  if (state.incorrectCount > 0) {
    state.incorrectCount++;
    state.currentKeyStatus = KeyStroke::INCORRECT;
  } else {
    if (ch == state.targetSequence[state.charCount]) {
      state.correctCount++;
      state.totalCorrect++;
      state.currentKeyStatus = KeyStroke::CORRECT;
    } else {
      state.incorrectCount++;
      state.currentKeyStatus = KeyStroke::INCORRECT;
    }
  }
  state.charCount++;
  if (ch == ' ' && state.incorrectCount == 0) {
    state.userInputSequence.clear();
  }
  spdlog::info("{} (CoC: {},ICC: {}, CaC: {}, BC: {}, CS: {}, TC: {}, TP: {})",
               ch, state.correctCount, state.incorrectCount, state.charCount,
               state.backspaceCount,
               state.userInputSequence.empty()
                   ? "_"
                   : std::string(state.userInputSequence.begin(),
                                 state.userInputSequence.end()),
               state.totalCorrect, state.totalPressed);
  return 0;
}

int inputValidator::get_results(State &state) {
  // memset(&state.result, '\0', sizeof(state.result));
  state.result.timeTaken = state.config.time;

  int totalErrors = state.totalPressed - state.totalCorrect;
  double words = state.totalCorrect / 5.0;

  state.result.netWPM = 60 * (words / state.result.timeTaken);
  state.result.rawWPM = 60 * (state.totalPressed / state.result.timeTaken);

  state.result.total_characters = state.totalPressed;
  state.result.correct_characters = state.totalCorrect;

  state.result.accuracy =
      static_cast<int>(100.0 * state.totalCorrect / state.totalPressed);

  spdlog::info("Results calculated: Time: {} Total Correct: {} Total Errors: "
               "{} WPM: {} Accuracy: {}",
               state.config.time, state.totalCorrect, totalErrors,
               state.result.netWPM, state.result.accuracy);
  return 0;
}
