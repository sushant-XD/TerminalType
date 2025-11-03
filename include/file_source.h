#pragma once

#include "configurations.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
class fileOps {
public:
  fileOps(std::string fileNameAbs);
  ~fileOps();

  error_e setup();
  error_e readFileContents();

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
};
