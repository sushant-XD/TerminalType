#include "main.h"
#include <chrono>

using namespace std::chrono_literals;
using namespace std::chrono;

int main(int argc, char *argv[]) {
  config_t config;
  std::fstream inFile;
  state_t state;

  state.isRunning = false;
  if (argc == 1) {
    if (!configure(config)) {
      return 1;
    }
  } else {
    if (!configure(argc, argv, config)) {
      return 1;
    }
  }

  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;

  fileOps fileManager(config.filePathAbs);
  terminalCtrl terminalManager;
  fileManager.setup(state);
  inputValidator inputValidator(terminalManager);
  screenState renderManager(terminalManager);

  renderManager.renderGradientBox(state);

  char tempChar;
  while (true) {
    tempChar = terminalManager.getCharacter();
    if (!state.isRunning) {
      state.startTime = steady_clock::now();
      state.isRunning = true;
    }

    auto elapsed =
        duration_cast<seconds>(steady_clock::now() - state.startTime);
    state.remainingTimeSeconds = state.totalTimeSeconds - elapsed.count();
    inputValidator.getInputAndCompare(state, tempChar);
    renderManager.renderTextProgress(state);
    renderManager.updateStats(state);
    if (elapsed.count() >= state.totalTimeSeconds) {
      break;
    }
  }

  state.isRunning = false;
  // result_t res = inputValidator.getResult(time);
  // renderManager.testComplete();
  std::cout << "\n\n============================ Test Complete ========="
            << std::endl;
  inputValidator.print_result();
  return 0;
}

bool configure(int size, char **args, config_s &config) {
  if (size == 1) {
    return configure(config);
  }
  for (int i = 1; i < size; i++) {
    std::string arg = args[i];

    if (arg == "-h" || arg == "--help") {
      config.reset();
      print_usage();
      return false;
    } else {
      if (i + 1 >= size) {
        std::cout << "Invalid Configuration." << std::endl;
        config.reset();
        print_usage();
        return false;
      }
      if (arg == "-f" || arg == "--file") {
        // TODO: add the absolute file path here
        config.filePathAbs = args[++i];
      } else if (arg == "-t" || args[i] == "--time") {
        config.time = std::stoi(args[++i]);
      } else if (arg == "-l" || arg == "--level") {
        int tempLevel = std::stoi(args[++i]);
        switch (tempLevel) {
        case 1:
          config.level = EASY;
          break;
        case 2:
          config.level = MEDIUM;
          break;
        case 3:
          config.level = HARD;
          break;
        default:
          std::cout << "Invalid level provided. Using Medium" << std::endl;
          break;
        }
      }
    }
  }

  if (config.filePathAbs.empty()) {
    config.filePathAbs = "testFile.txt";
  }
  return true;
}

bool configure(config_s &config) {
  config.filePathAbs = "testFile.txt";
  config.time = DEFAULT_TIME;
  config.level = DEFAULT_LEVEL;
  return true;
}

void print_config(config_s &config) {
  std::cout << "===========User Settings Config=======" << std::endl;
  std::cout << "File Path :     " << config.filePathAbs << std::endl;
  std::cout << "Test Time :     " << config.time << std::endl;
  std::cout << "Test Level:     " << config.level << std::endl;
  return;
}

void print_usage() {
  std::cout << "Usage: terminalType [options]" << std::endl;
  std::cout << "-f, --file          "
            << "File Name if words are in a CSV or text file" << std::endl;
  std::cout << "-t, --time          "
            << "Time (in seconds) to run the Typing Test" << std::endl;
  std::cout << "-l, --level         "
            << "Level - 1, 2, 3, etc. (if default words dictionary is used). "
               "ignored if custom "
               "file provided"
            << std::endl;
}
