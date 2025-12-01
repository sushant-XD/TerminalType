#pragma once

#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

// File Errors for File Operations Only
enum class FileError { OK = 0, CANNOT_OPEN, INVALID_SIZE, CANNOT_READ };

enum class KeyStroke { BACKSPACE = 0, BACK_WORD, CORRECT, INCORRECT };
enum class TestState { RUNNING = 0, MENU, SETTINGS, RESULTS };
enum class MenuOpts { START = 0, SETTINGS, QUIT };
enum class ResultOpts { MENU = 0, RESTART, QUIT };

// easy mode is top 200 words, mdium is top 500 words, hard is top 1000 words,
// Custom is if they select their own file
enum class Level { EASY, MEDIUM, HARD, CUSTOM };

static constexpr Level DEFAULT_LEVEL = Level::EASY;
static constexpr int DEFAULT_TIME = 10;
static constexpr int MIN_FILE_SIZE_WORDS = 100;
static constexpr char BACKSPACE[3] = {'\b', ' ', '\b'};

struct Config {
  std::string filePathAbs; // absolute file path (if given by the user)
  int time;    // time selected by the user in seconds, default is 60
  Level level; // mode selected, default is EASY

  void reset() {
    filePathAbs.clear();
    time = 0;
    level = Level::EASY;
  }
};

struct Result {
  int result_id;
  int timeTaken;
  Level level;
  int correct_characters;
  int total_characters;
  int accuracy;
};

struct Screen {
  int width;
  int height;
};

struct State {
  TestState currentState;
  Result result;
  Config config;

  int correctCount;
  int charCount;
  int incorrectCount;

  KeyStroke currentKeyStatus;

  std::vector<char> targetSequence;
  std::vector<std::string> wrappedLines;

  std::vector<char> userInputSequence;

  Screen ws; // window size

  std::chrono::steady_clock::time_point startTime;

  int totalTimeSeconds;
  int remainingTimeSeconds;

  int backspaceCount;
};

/* Constants */
