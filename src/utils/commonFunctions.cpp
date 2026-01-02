#include "utils/commonFunctions.h"

std::string selectOptionInList(std::vector<std::string> optionsList,
                               int option) {
  std::vector<std::string> options = optionsList;
  if (option >= 0 && option < options.size()) {
    options[option] = "\t> " + options[option];
  }
  std::string optionsString;
  for (const auto &line : options) {
    optionsString += line + "\n";
  }
  return optionsString;
}
