#include "main.h"
#include "logging.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

int main(int argc, char *argv[]) {

  config_t config;
  std::fstream inFile;
  state_t state;

  state.currentState = testState::MENU;
  if (argc == 1) {
    if (!configure(config)) {
      return 1;
    }
  } else {
    if (!configure(argc, argv, config)) {
      return 1;
    }
  }
  try {
    init_logger("my_app_logger", "logs/logTest.log", spdlog::level::debug);
  } catch (const spdlog::spdlog_ex &ex) {
    return 1;
  }

  initializeState(state);

  spdlog::info("Initializing Terminal Typing Program");
  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;

  spdlog::info("Total Time: {} ", config.time);
  fileOps fileManager(config.filePathAbs);
  terminalCtrl terminalManager;
  fileManager.setup(state);

  inputValidator inputValidator(terminalManager);
  // screenState renderManager(terminalManager);

  // renderManager.renderMenuScreen(state);
  spdlog::info("Initial Screen Rendering Complete");

  std::chrono::steady_clock::time_point statsUpdateTime;
  char tempChar = '\0';
  selectedMenuOption selectedSetting;
  while (true) {

    tempChar = terminalManager.getCharacter();

    if (state.currentState == testState::RUNNING) {
      if (state.startTime == std::chrono::steady_clock::time_point()) {
        state.startTime = steady_clock::now();
        statsUpdateTime = steady_clock::now();
        renderManager.renderStartScreen(state);
      }

      // if there's no input, then tempChar is '\0'
      if (tempChar != '\0') {
        inputValidator.getInputAndCompare(state, tempChar);
      }

      auto elapsed =
          duration_cast<seconds>(steady_clock::now() - state.startTime);
      state.remainingTimeSeconds = state.totalTimeSeconds - elapsed.count();

      // time up, set currentState to Results and exit
      if (elapsed.count() >= state.totalTimeSeconds) {
        state.currentState = testState::RESULTS;
      } else {
        // if there's no input, then tempChar is '\0'
        if (tempChar != '\0') {
          renderManager.renderTextProgress(state);
        }
        if (duration_cast<seconds>(steady_clock::now() - statsUpdateTime) >=
            duration(1s)) {
          renderManager.updateStats(state);
          statsUpdateTime = steady_clock::now();
        }
        std::this_thread::sleep_for(5ms);
      }
    } else if (state.currentState == testState::MENU) {
      renderManager.renderMenuScreen(state);
      if (tempChar == 'j' || tempChar == 'J') {
        terminalManager.moveCursorDown();
        selectedSetting = renderManager.updateMenuScreen();
      } else if (tempChar == 'k' || tempChar == 'K') {
        terminalManager.moveCursorUp();
        selectedSetting = renderManager.updateMenuScreen();
      } else if (tempChar == '\n') {
        spdlog::info("Enter key pressed. Selected Option: {}",
                     (int)selectedSetting);
        if (selectedSetting == selectedMenuOption::START) {
          state.currentState = testState::RUNNING;
          terminalManager.getAllCharacters();
        } else if (selectedSetting == selectedMenuOption::SETTINGS) {
          state.currentState = testState::SETTINGS;
        } else if (selectedSetting == selectedMenuOption::QUIT) {
          spdlog::info("Qutting...");
          break;
        }
      }
    } else if (state.currentState == testState::RESULTS) {
      renderManager.get_and_print_result(state);
      std::this_thread::sleep_for(duration(3s));
      // clear any terminal key that might accidentally trigger the game again
      char *tmpBuf = terminalManager.getAllCharacters();
      if (tmpBuf != nullptr) {
        spdlog::debug("Cleared {} characters to flush", sizeof(tmpBuf));
      } else {
        spdlog::debug("Nothing to clear in the terminal Buffer");
      }
    } else if (state.currentState == testState::SETTINGS) {
      spdlog::info("Inside Settings Menu");
    } else {
      spdlog::error("Invalid State");
      break;
    }
    std::this_thread::sleep_for(5ms);
  }
  renderManager.clearTerminal();
  return 0;
}

void initializeState(state_t &state) {
  state.currentState = testState::MENU;
  state.result = {};
  state.correctCount = 0;
  state.incorrectCount = 0;
  state.charCount = 0;
  state.currentKeyStatus = keyStroke::CORRECT;
  state.userInputSequence = std::vector<char>(10, '\0');
  state.totalTimeSeconds = 0;
  state.remainingTimeSeconds = 0;
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
