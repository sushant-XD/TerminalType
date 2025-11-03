#include "main.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

int main(int argc, char *argv[]) {
  config_t config;
  std::fstream inFile;
  std::vector<char> userInput;

  if (argc == 1) {
    if (!configure(config)) {
      return 1;
    }
  } else {
    if (!configure(argc, argv, config)) {
      return 1;
    }
  }

  fileOps fileManager(config.filePathAbs);
  terminalCtrl terminalManager;
  // TODO: handle incorrect setup, give chance to user to re-enter
  fileManager.setup();
  int width = terminalManager.getTerminalWidth();
  print_welcome_screen(width);
  print_settings(width, config);
  std::cout << std::string(3, ' ') << BBLACK << std::string(width - 4, '-')
            << COLOR_RESET << std::endl;
  fileManager.printFileContents();

  inputValidator inputValidator(fileManager.characters, terminalManager);

  auto timeIntervalms = milliseconds(config.time * 1000);
  char tempChar;

  // TODO: handle backspace
  int i = 0;
  int correctCount = 0;
  std::cout << std::endl;

  bool started = false;
  steady_clock::time_point startTime;

  while (true) {
    tempChar = terminalManager.getCharacter();
    if (!started) {
      startTime = steady_clock::now();
      started = true;
    }
    inputValidator.getInputAndCompare(tempChar);
    if (started && duration_cast<milliseconds>(steady_clock::now() -
                                               startTime) >= timeIntervalms) {
      break;
    }
  }

  int time = duration_cast<milliseconds>(timeIntervalms).count() / 1000;
  result_t res = inputValidator.getResult(time);

  std::cout << "\n\n============================ Test Complete ========="
            << std::endl;
  inputValidator.print_result();
  return 0;
}

void print_welcome_screen(int width) {
  int padding = 2;
  int actualWidth = width - (padding * 3);
  std::string title = "Terminal Typing Test";
  int leftRightMargin = (actualWidth - title.size()) / 2;
  std::cout << std::string(padding, ' ') << "+" << std::string(actualWidth, '-')
            << "+" << std::endl;
  // std::cout << std::string(padding, ' ') << "|" << std::string(actualWidth, '
  // ') << "|" << std::endl;
  std::cout << std::string(padding, ' ') << "|"
            << std::string(leftRightMargin, ' ') << title
            << std::string(leftRightMargin, ' ') << "|" << std::endl;
  // std::cout << std::string(padding, ' ') << "|" << std::string(actualWidth, '
  // ') << "|" << std::endl;
  std::cout << std::string(padding, ' ') << "+" << std::string(actualWidth, '-')
            << "+" << std::endl;
}

void print_settings(int width, config_s &config) {
  int padding = 4;
  int innerPadding = 6;
  int actualWidth = width - (padding * 2);

  std::string left = "Time: " + std::to_string(config.time) + "s";
  std::string right = "Level: " + std::to_string(config.level);
  std::string middle = "|";

  int totalContentWidth =
      left.size() + middle.size() + right.size() + innerPadding * 2;
  int sideSpace = (actualWidth - totalContentWidth) / 2;

  std::cout << std::string(sideSpace, ' ') << left
            << std::string(innerPadding, ' ') << middle
            << std::string(innerPadding, ' ') << right << std::endl;
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
