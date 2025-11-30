#include "fileManagement/file_source.h"

fileOps::fileOps(std::string fileNameAbs) {

  if (fileNameAbs.empty()) {
    throw std::runtime_error("File Name is empty");
  }

  fileName = fileNameAbs;
}

fileOps::~fileOps() {
  if (inFile.is_open()) {
    inFile.close();
  }
}

error_e fileOps::setup(state_t &state) {
  inFile.open(fileName);
  if (!inFile.is_open()) {
    std::cout << "Unable to open file" << fileName << std::endl;
    return FILE_READ_ERROR;
  }

  if (readFileContents() != OK) {
    std::cout << "Unable to read file contents" << std::endl;
    return FILE_READ_ERROR;
  }

  if (wordCount < MIN_FILE_SIZE_WORDS) {
    std::cout << "Too Small File. " << std::endl;
    return FILE_SIZE_ERROR;
  }

  state.targetSequence = characters;
  return OK;
}

error_e fileOps::readFileContents() {
  if (!inFile.is_open()) {
    std::cout << "File Not opened to read" << std::endl;
    return FILE_READ_ERROR;
  }
  std::string tempStr = "";
  while (std::getline(inFile, tempStr)) {
    words.push_back(tempStr);
    for (auto ch : tempStr) {
      characters.push_back(ch);
    }
    words.push_back(" ");
    characters.push_back(' ');
  }

  words.pop_back();
  characters.pop_back();

  if (words.size() < MIN_FILE_SIZE_WORDS) {
    return FILE_SIZE_ERROR;
  }

  wordCount = words.size();
  charCount = characters.size();

  return OK;
}

void fileOps::printFileContents() {
  for (auto ch : characters) {
    std::cout << ch;
  }
  std::cout << std::endl;
}
