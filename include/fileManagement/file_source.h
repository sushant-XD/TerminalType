#pragma once

#include "utils/configurations.h"
#include <algorithm>
#include <fstream>
#include <random>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <vector>

class fileOps {
public:
  fileOps(std::string fileNameAbs);
  ~fileOps();

  FileError setup(State &state);

  FileError refresh(State &state);
  // getters for word count and character counts
  void printFileContents();
  int getWordCount() { return wordCount; }
  int getCharCount() { return charCount; }

  std::vector<char> characters;

private:
  std::vector<std::string> words;

  std::string fileName;
  std::fstream inFile;
  int wordCount;
  int charCount;

  std::random_device rd;

  bool readFileContents();
  void copyWordsToCharacters();
  void shuffle();
};
