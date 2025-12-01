#include "main.h"
#include "logging.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

int main(int argc, char *argv[]) {

  Config config;
  std::fstream inFile;
  State state;

  state.currentState = TestState::MENU;
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

  initializeState(state, config);

  spdlog::info("Initializing Terminal Typing Program");
  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;

  spdlog::info("Total Time: {} ", config.time);
  fileOps fileManager(config.filePathAbs);
  terminalCtrl terminalManager;
  if (fileManager.setup(state) != FileError::OK) {
    spdlog::error("Couldn't read file contents.");
    return 1;
  }

  inputValidator inputValidator(terminalManager);
  screenManager renderManager(terminalManager);

  spdlog::info("Initial Screen Rendering Complete");

  std::chrono::steady_clock::time_point statsUpdateTime;
  char tempChar = '\0';
  MenuOpts selectedSetting;
  ResultOpts selectedResultOption;
  while (true) {
    tempChar = terminalManager.getCharacter();

    switch (state.currentState) {
    case TestState::RUNNING:
      handleRunningState(state, tempChar, renderManager, inputValidator,
                         terminalManager, statsUpdateTime);
      break;

    case TestState::MENU:
      handleMenuState(state, tempChar, renderManager, terminalManager,
                      fileManager, selectedSetting);
      break;

    case TestState::RESULTS:
      handleResultsState(state, tempChar, renderManager, terminalManager,
                         fileManager, selectedResultOption);
      break;

    case TestState::SETTINGS:
      handleSettingsState(state);
      break;

    default:
      spdlog::error("Invalid State");
      renderManager.clearTerminal();
      return 1;
    }

    std::this_thread::sleep_for(5ms);
  }

  renderManager.clearTerminal();
  return 0;
}

void handleRunningState(
    State &state, char tempChar, screenManager &renderManager,
    inputValidator &validator, terminalCtrl &terminal,
    std::chrono::steady_clock::time_point &statsUpdateTime) {
  // Initialize start time on first entry
  if (state.startTime == std::chrono::steady_clock::time_point()) {
    state.startTime = steady_clock::now();
    statsUpdateTime = steady_clock::now();
    renderManager.switchToScreen(TestState::RUNNING);
    renderManager.render(state);
  }

  // Process input
  if (tempChar != '\0') {
    validator.getInputAndCompare(state, tempChar);
    renderManager.update(state);
  }

  // Update time
  auto elapsed = duration_cast<seconds>(steady_clock::now() - state.startTime);
  state.remainingTimeSeconds = state.totalTimeSeconds - elapsed.count();

  // Check if time is up
  if (elapsed.count() >= state.totalTimeSeconds) {
    state.currentState = TestState::RESULTS;
    state.startTime = std::chrono::steady_clock::time_point();
    spdlog::info("Time up. Switching to Results");
    renderManager.switchToScreen(TestState::RESULTS);
    return;
  }

  // Update stats every second
  if (duration_cast<seconds>(steady_clock::now() - statsUpdateTime) >= 1s) {
    renderManager.updateStats(state);
    statsUpdateTime = steady_clock::now();
  }
}

void handleMenuState(State &state, char tempChar, screenManager &renderManager,
                     terminalCtrl &terminal, fileOps &fileManager,
                     MenuOpts &selectedSetting) {
  // Render menu if needed
  if (renderManager.needsRender()) {
    spdlog::info("Menu Screen needs re-render");
    renderManager.switchToScreen(TestState::MENU);
    renderManager.render(state);
  } else {
    spdlog::info("Menu didn't need re-render");
  }

  // Handle navigation
  if (tempChar == 'j' || tempChar == 'J') {
    spdlog::info("Moving Cursor Down");
    selectedSetting = renderManager.updateMenuSelection(false);
  } else if (tempChar == 'k' || tempChar == 'K') {
    spdlog::info("Moving Cursor Up");
    selectedSetting = renderManager.updateMenuSelection(true);
  } else if (tempChar == '\n') {
    spdlog::info("Enter key pressed. Selected Option: {}",
                 static_cast<int>(selectedSetting));

    switch (selectedSetting) {
    case MenuOpts::START:
      state.currentState = TestState::RUNNING;
      renderManager.switchToScreen(TestState::RUNNING);
      terminal.getAllCharacters(); // Clear input buffer
      fileManager.refresh(state);
      spdlog::info("Starting Game from Menu.");
      break;

    case MenuOpts::SETTINGS:
      state.currentState = TestState::SETTINGS;
      renderManager.switchToScreen(TestState::SETTINGS);
      spdlog::info("Switching from Menu to settings.");
      break;

    case MenuOpts::QUIT:
      spdlog::info("Quitting...");
      renderManager.clearTerminal();
      exit(0);
      break;
    }
  }
}

void handleResultsState(State &state, char tempChar,
                        screenManager &renderManager, terminalCtrl &terminal,
                        fileOps &fileManager, ResultOpts &selectedOption) {
  // Render results if needed
  if (renderManager.needsRender()) {
    spdlog::info("Results Screen needs render");
    renderManager.render(state);
  }

  // Handle navigation
  if (tempChar == 'j' || tempChar == 'J') {
    spdlog::info("Moving Cursor Down");
    selectedOption = renderManager.updateResultsSelection(false);
  } else if (tempChar == 'k' || tempChar == 'K') {
    spdlog::info("Moving Cursor Up");
    selectedOption = renderManager.updateResultsSelection(true);
  } else if (tempChar == '\n') {
    spdlog::info("Enter key pressed. Selected Option: {}",
                 static_cast<int>(selectedOption));

    switch (selectedOption) {
    case ResultOpts::MENU:
      // Clear input buffer
      terminal.getAllCharacters();
      // Reset state and return to menu
      configure(state.config);
      initializeState(state, state.config);
      state.currentState = TestState::MENU;
      renderManager.switchToScreen(TestState::MENU);
      spdlog::info("Returning to Menu");
      break;

    case ResultOpts::RESTART:
      // Clear input buffer
      terminal.getAllCharacters();
      // Reset state and restart test
      configure(state.config);
      initializeState(state, state.config);
      fileManager.refresh(state);

      state.currentState = TestState::RUNNING;
      renderManager.switchToScreen(TestState::RUNNING);
      spdlog::info("Restarting Test");
      break;

    case ResultOpts::QUIT:
      spdlog::info("Quitting...");
      renderManager.clearTerminal();
      exit(0);
      break;
    }
  }
}

void handleSettingsState(State &state) {
  spdlog::info("Inside Settings Menu");
  // TODO: Implement settings screen logic
}

void initializeState(State &state, Config config) {
  state.currentState = TestState::MENU;
  state.result = {};
  state.config.time = 10;
  state.config.level = Level::EASY;
  state.correctCount = 0;
  state.incorrectCount = 0;
  state.charCount = 0;
  state.currentKeyStatus = KeyStroke::CORRECT;
  state.userInputSequence = std::vector<char>(10, '\0');
  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;
  state.startTime = std::chrono::steady_clock::time_point();
  spdlog::info("State Reset. All variables Reset.");
}

bool configure(int size, char **args, Config &config) {
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
          config.level = Level::EASY;
          break;
        case 2:
          config.level = Level::MEDIUM;
          break;
        case 3:
          config.level = Level::HARD;
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

bool configure(Config &config) {
  config.filePathAbs = "testFile.txt";
  config.time = DEFAULT_TIME;
  config.level = DEFAULT_LEVEL;
  return true;
}

void print_config(Config &config) {
  std::cout << "===========User Settings Config=======" << std::endl;
  std::cout << "File Path :     " << config.filePathAbs << std::endl;
  std::cout << "Test Time :     " << config.time << std::endl;
  std::cout << "Test Level:     " << static_cast<int>(config.level)
            << std::endl;
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
