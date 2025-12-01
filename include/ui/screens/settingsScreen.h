
#pragma once
#include "ui/screens/canvas.h"

enum class SettingOption { TIME = 0, LEVEL = 1, BACK = 2 };

class settingsScreen : public Canvas {
private:
  bool isRendered;
  SettingOption currentSelected;
  std::vector<std::string> settingOptionList;
  int settingOptionsNum;

  int headerStartX;
  int headerStartY;
  int headerWidth;
  int headerHeight;

  int settingsOptionsStartX;
  int settingsOptionsStartY;
  int settingsOptionsWidth;
  int settingsOptionsHeight;

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
