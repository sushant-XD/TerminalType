#include "fileManagement/file_source.h"
#include <filesystem>
fileOps::fileOps(std::string fileNameAbs) {
  assert(!fileNameAbs.empty());
  fileName = fileNameAbs;
}

fileOps::~fileOps() {
  if (inFile.is_open()) {
    inFile.close();
  }
}

FileError fileOps::setup(State &state) {

  // clear flags and close the file if it was attempted to open already
  inFile.clear();
  if (inFile.is_open()) {
    inFile.close();
  }
  inFile.open(fileName, std::ios::in);
  if (!inFile.is_open()) {
    std::cerr << "Coulnd't open file: " << fileName << std::endl;
    return FileError::CANNOT_OPEN;
  }

  if (!readFileContents()) {
    std::cerr << "Couldn't read file contents from" << fileName << std::endl;
    return FileError::CANNOT_READ;
  }

  if (wordCount < MIN_FILE_SIZE_WORDS) {
    std::cerr << "File too small, not enough words" << fileName << std::endl;
    return FileError::INVALID_SIZE;
  }
  shuffle();
  copyWordsToCharacters();
  state.targetSequence = characters;
  return FileError::OK;
}

FileError fileOps::refresh(State &state) {
  if (fileName != state.config.filePathAbs) {
    inFile.close();
    fileName = state.config.filePathAbs;
    inFile.open(fileName);
    if (!inFile.is_open()) {
      return FileError::CANNOT_OPEN;
    }
    if (!readFileContents()) {
      return FileError::CANNOT_READ;
    }
    if (wordCount < MIN_FILE_SIZE_WORDS) {
      return FileError::INVALID_SIZE;
    }
  }
  // shuffle and return
  shuffle();
  copyWordsToCharacters();
  state.targetSequence = characters;
  return FileError::OK;
}

// words are shuffled and the characters vector is updated
// premise of shuffle algorithm is:
void fileOps::shuffle() {
  std::mt19937 g(rd());
  if (words.size() < 9) {
    spdlog::error("Not enough words to shuffle.");
    return;
  }
  std::shuffle(words.begin(), words.end(), g);
}

bool fileOps::readFileContents() {
  if (!inFile.is_open()) {
    std::cout << "File Not opened to read" << std::endl;
    return false;
  }
  std::string tempWord;
  while (inFile >> tempWord) {
    words.push_back(tempWord);
  }
  wordCount = words.size();
  return true;
}

void fileOps::copyWordsToCharacters() {
  if (words.empty()) {
    std::cerr << "No words to open" << std::endl;
    return;
  }
  if (!characters.empty()) {
    characters.clear();
  }

  for (size_t i = 0; i < words.size(); ++i) {
    characters.insert(characters.end(), words[i].begin(), words[i].end());
    if (i < words.size() - 1) {
      characters.push_back(' ');
    }
  }
}

void fileOps::printFileContents() {
  for (auto ch : characters) {
    std::cout << ch;
  }
  std::cout << std::endl;
}
