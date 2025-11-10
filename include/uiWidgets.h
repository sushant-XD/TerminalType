#pragma once
#include "design.h"
#include "terminal_ctrl.h"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

enum class uiError {
  OK,
  DRAW_DIM_ERROR,
  REFRESH_ERROR,
  ERASE_ERROR,
  UI_ALREADY_DRAWN
};

enum class borderShape {
  SHARP_SINGLE,
  SHARP_DOUBLE,
  ROUNDED_SINGLE,
  ROUNDED_DOUBLE
};

struct borderChars {
  const char *topLeft;
  const char *topRight;
  const char *bottomLeft;
  const char *bottomRight;
  const char *horizontal;
  const char *vertical;
};

class uiWidget {
public:
  uiWidget(int windowWidth, int windowHeight, terminalCtrl &terminalManager);
  ~uiWidget();
  uiError drawBox(int startCol, int startRow, int width, int height,
                  bool centerAlign, borderShape shape, char *borderColor,
                  bool isStatic);
  uiError drawLine(int x, int y, int width, int height, bool centerAlign,
                   char *linShape, char *linColor, bool isStatic);
  uiError drawBox(int startCol, int startRow, int width, int height,
                  std::string text, bool centerAlign, borderShape shape,
                  char *borderColor, bool isStatic);
  uiError refresh();
  uiError erase();

private:
  bool widgetDrawn;
  bool isStatic;
  bool isTextBox;
  terminalCtrl &terminalManager;

  int windowWidth;
  int windowHeight;

  int x;
  int y;
  int width;
  int height;
  int textStartRow;
  int textStartCol;

  std::vector<std::string> wrapText(std::string &text, int maxWidth);
  borderChars getBorderChars(borderShape shape);
};
