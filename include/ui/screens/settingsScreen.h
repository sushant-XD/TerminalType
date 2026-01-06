
#pragma once
#include "ui/screens/canvas.h"

enum class SettingOption { TIME = 0, LEVEL = 1, BACK = 2 };

struct SettingLayout {
  Box header;
  Box options;
  SettingLayout(int canvasX, int canvasY, int canvasWidth, int canvasHeight,
                int numOfOptions) {
    header = {
        .x = canvasX + 1,
        .y = canvasY + 1,
        .width = canvasWidth - 2,
        .height = 3,
    };
    options = {
        .x = canvasX + 1,
        .y = header.y + header.height,
        .width = canvasWidth - 2,
        .height = numOfOptions * 2 + 2,
    };
  };
};

class settingsScreen : public Canvas {
private:
  bool isRendered;
  SettingOption currentSelected;
  std::vector<std::string> settingOptionList;
  int settingOptionsNum;

  SettingLayout layout;

  uiWidget header;
  uiWidget settingsOptions;

  std::string getSettingsString(const State &state);

public:
  settingsScreen(terminalCtrl &terminal);
  void render(State &state) override;
  void update(const State &state) override;
  void clear() override;
  SettingOption updateSelection(bool up);
  SettingOption getCurrentSelection() const { return currentSelected; }
};
