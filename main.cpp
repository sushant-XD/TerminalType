#include "main.h"
#include "include/utils/configurations.h"
#include "logging.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

std::atomic<bool> shutdown_requested(false);

void signalHandler(int signum) {
  std::cout << "\nInterrupt signal (" << signum << ") received.\n";
  shutdown_requested.store(true);
}

int main(int argc, char *argv[]) {

  Config config;
  std::fstream inFile;
  State state;

  signal(SIGINT, signalHandler);

  state.currentState = TestState::MENU;
  if (argc == 1) {
    if (!configure(config)) {
      std::cerr << "configuration failed." << std::endl;
      return 2;
    }
  } else {
    if (!configure(argc, argv, config)) {
      std::cerr << "configuration failed." << std::endl;
      return 3;
    }
  }

  std::filesystem::create_directories("logs");
  try {
    init_logger("my_app_logger", "logs/logTest.log", spdlog::level::debug);
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Logger Initialization failed: " << ex.what() << std::endl;
    return 1;
  }

  initializeState(state, config);

  // spdlog::info("Initializing Terminal Typing Program");

  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;

  // spdlog::info("Total Time: {} ", config.time);
  fileOps fileManager(config.filePathAbs);
  terminalCtrl terminalManager;

  // Check for hidden characters
  for (char c : config.filePathAbs) {
    if (c < 32 || c > 126) {
      std::cerr << "WARNING: Non-printable character found: " << (int)c
                << std::endl;
      return 5;
    }
  }

  if (fileManager.setup(state) != FileError::OK) {
    spdlog::error("Couldn't read file contents.");
    return 4;
  }
  inputValidator inputValidator(terminalManager);
  screenManager renderManager(terminalManager);

  // spdlog::info("Initial Screen Rendering Complete");
  std::chrono::steady_clock::time_point statsUpdateTime;
  char tempChar = '\0';
  MenuOpts selectedSetting;
  ResultOpts selectedResultOption;
  SettingOption selectedSettingOption;

  while (!shutdown_requested.load()) {
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
      handleSettingsState(state, tempChar, renderManager,
                          selectedSettingOption);
      break;

    default:
      spdlog::error("Invalid State");
      renderManager.clearTerminal();
      return 1;
    }

    std::this_thread::sleep_for(5ms);
  }

  // renderManager.clearTerminal();
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
    spdlog::info("Running state started");
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
    validator.get_results(state);
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
    spdlog::info("Re-rendering Menu");
    renderManager.switchToScreen(TestState::MENU);
    renderManager.render(state);
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

      state.totalTimeSeconds = state.config.time;
      state.remainingTimeSeconds = state.config.time;

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
      shutdown_requested.store(true);
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
      initializeState(state, state.config);
      state.currentState = TestState::MENU;
      renderManager.switchToScreen(TestState::MENU);
      spdlog::info("Returning to Menu");
      break;

    case ResultOpts::RESTART:
      // Clear input buffer
      terminal.getAllCharacters();
      // Reset state and restart test
      initializeState(state, state.config);
      fileManager.refresh(state);

      state.currentState = TestState::RUNNING;
      renderManager.switchToScreen(TestState::RUNNING);
      spdlog::info("Restarting Test");
      break;

    case ResultOpts::QUIT:
      spdlog::info("Quitting...");
      renderManager.clearTerminal();
      shutdown_requested.store(true);
      break;
    }
  }
}

void handleSettingsState(State &state, char tempChar,
                         screenManager &renderManager,
                         SettingOption &selectedOption) {
  // Render settings if needed
  if (renderManager.needsRender()) {
    spdlog::info("Settings Screen needs render");
    renderManager.switchToScreen(TestState::SETTINGS);
    renderManager.render(state);
  }

  // Handle navigation
  if (tempChar == 'j' || tempChar == 'J') {
    spdlog::info("Moving Cursor Down");
    selectedOption = renderManager.updateSettingsSelection(false);
    renderManager.update(state);
  } else if (tempChar == 'k' || tempChar == 'K') {
    spdlog::info("Moving Cursor Up");
    selectedOption = renderManager.updateSettingsSelection(true);
    renderManager.update(state);
  } else if (tempChar == 'h' || tempChar == 'H') {
    spdlog::info("Decreasing value");
    renderManager.modifySettingValue(state, false);
  } else if (tempChar == 'l' || tempChar == 'L') {
    spdlog::info("Increasing value");
    renderManager.modifySettingValue(state, true);
  } else if (tempChar == '\n') {
    spdlog::info("Enter key pressed. Selected Option: {}",
                 static_cast<int>(selectedOption));

    if (selectedOption == SettingOption::BACK) {
      state.currentState = TestState::MENU;
      renderManager.switchToScreen(TestState::MENU);
      spdlog::info("Returning to Menu from Settings");
    }
  }
}

void initializeState(State &state, Config config) {

#ifdef INSTALL_PREFIX
  std::string dataDir =
      std::string(INSTALL_PREFIX) + "/share/" + PROJECT_NAME + "/";
#else
  std::string dataDir = "";
#endif

  state.currentState = TestState::MENU;
  state.result = {};
  state.config.time = config.time;
  state.config.level = config.level;
  state.correctCount = 0;
  state.incorrectCount = 0;
  state.charCount = 0;
  state.currentKeyStatus = KeyStroke::CORRECT;
  state.userInputSequence = std::vector<char>(10, '\0');
  state.totalTimeSeconds = config.time;
  state.remainingTimeSeconds = config.time;
  state.startTime = std::chrono::steady_clock::time_point();

  state.totalCorrect = 0;
  state.totalPressed = 0;
  state.result.accuracy = 0;

  if (state.config.level == Level::MEDIUM) {
    state.config.filePathAbs = dataDir + "MediumLevel.txt";
  } else if (state.config.level == Level::HARD) {
    state.config.filePathAbs = dataDir + "HardLevel.txt";
  } else {
    state.config.filePathAbs = dataDir + "EasyLevel.txt";
  }
  spdlog::info("State Reset. All variables Reset.");
}

bool configure(int size, char **args, Config &config) {
  if (size == 1) {
    return configure(config);
  }

#ifdef INSTALL_PREFIX
  std::string dataDir =
      std::string(INSTALL_PREFIX) + "/share/" + PROJECT_NAME + "/";
#else
  std::string dataDir = "";
#endif

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
    if (config.level == Level::MEDIUM) {
      config.filePathAbs = dataDir + "MediumLevel.txt";
    } else if (config.level == Level::HARD) {
      config.filePathAbs = dataDir + "HardLevel.txt";
    } else {
      config.filePathAbs = dataDir + "EasyLevel.txt";
    }
  }
  return true;
}

bool configure(Config &config) {
#ifdef INSTALL_PREFIX
  std::string dataDir =
      std::string(INSTALL_PREFIX) + "/share/" + PROJECT_NAME + "/";
#else
  std::string dataDir = "";
#endif

  config.filePathAbs = dataDir + "EasyLevel.txt"; // Set default here
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
