#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
// easy mode is top 200 words, mdium is top 500 words, hard is top 1000 words,
// Custom is if they select their own file
enum level_e { EASY, MEDIUM, HARD, CUSTOM };

enum error_e {
  OK = 0,
  FILE_READ_ERROR,
  USER_INPUT_ERROR,
  FILE_SIZE_ERROR,
  UNKNOWN_ERROR
};

typedef struct config_s {
  std::string filePathAbs; // absolute file path (if given by the user)
  int time;      // time selected by the user in seconds, default is 60
  level_e level; // mode selected, default is EASY

  void reset() {
    filePathAbs.clear();
    time = 0;
    level = EASY;
  }

} config_t;

typedef struct result_s {
  int result_id;
  int timeTaken;
  level_e level;
  int correct_characters;
  int total_characters;
  int accuracy;
} result_t;

enum class keyStroke { BACKSPACE = 0, CORRECT, INCORRECT };

typedef struct screen_s {
  int width;
  int height;
} screen_t;

typedef struct state_s {
  bool isRunning;
  result_t result;
  int correctCount;
  int charCount;
  int incorrectCount;
  keyStroke currentKeyStatus;
  screen_t ws;                      // not used right now
  std::vector<char> targetSequence; // not implemented right now
  std::vector<char> userInputSequence;

  std::chrono::steady_clock::time_point startTime;
  int totalTimeSeconds;
  int remainingTimeSeconds;
} state_t;

/* Constants */
static constexpr level_e DEFAULT_LEVEL = EASY;
static constexpr int DEFAULT_TIME = 10;
static constexpr int MIN_FILE_SIZE_WORDS = 100;
static constexpr char BACKSPACE[3] = {'\b', ' ', '\b'};
