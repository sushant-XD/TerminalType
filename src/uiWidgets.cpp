#include "uiWidgets.h"
#include <sstream>

uiWidget::uiWidget(int windowWidth, int windowHeight, terminalCtrl &terminalMgr)
    : terminalManager(terminalMgr), windowWidth(windowWidth),
      windowHeight(windowHeight) {
  widgetDrawn = false;
  isStatic = false;
  isTextBox = false;
  x = 0;
  y = 0;
  width = 0;
  height = 0;
}

uiWidget::~uiWidget() {}

uiError uiWidget::drawBox(int startCol, int startRow, int width, int height,
                          bool centerAlign, borderShape shape,
                          char *borderColor, bool isStatic) {
  if (widgetDrawn) {
    spdlog::error("Box already drawn. Not drawing at {} {}", startCol,
                  startRow);
    return uiError::UI_ALREADY_DRAWN;
  }
  this->isStatic = isStatic;
  int endCol = startCol + width;
  int endRow = startRow + height;
  if (endRow > windowHeight || endCol > windowWidth) {
    return uiError::DRAW_DIM_ERROR;
  }
  borderChars border = getBorderChars(shape);

  this->x = startCol;
  this->y = startRow;
  this->width = width;
  this->height = height;
  this->textStartCol = -1;
  terminalManager.hideCursor();
  terminalManager.moveCursor(startRow, startCol);
  for (int i = 0; i < height; i++) {
    terminalManager.moveCursor(startRow + i, startCol);
    for (int j = 0; j < width; j++) {
      if (i == 0) { // Top edge
        if (j == 0) {
          terminalManager.writeToTerminal((char *)border.topLeft,
                                          strlen(border.topLeft));
        } else if (j == width - 1) {
          terminalManager.writeToTerminal((char *)border.topRight,
                                          strlen(border.topRight));
        } else {
          terminalManager.writeToTerminal((char *)border.horizontal,
                                          strlen(border.horizontal));
        }
      } else if (i == height - 1) { // Bottom edge
        if (j == 0) {
          terminalManager.writeToTerminal((char *)border.bottomLeft,
                                          strlen(border.bottomLeft));
        } else if (j == width - 1) {
          terminalManager.writeToTerminal((char *)border.bottomRight,
                                          strlen(border.bottomRight));
        } else {
          terminalManager.writeToTerminal((char *)border.horizontal,
                                          strlen(border.horizontal));
        }
      } else { // Middle rows
        if (j == 0 || j == width - 1) {
          terminalManager.writeToTerminal((char *)border.vertical,
                                          strlen(border.vertical));
        } else {
          terminalManager.writeToTerminal((char *)" ", 1);
        }
      }
    }
  }
  widgetDrawn = true;
  return uiError::OK;
}

uiError uiWidget::drawBox(int startCol, int startRow, int width, int height,
                          std::string text, bool centerAlign, borderShape shape,
                          char *borderColor, char *textColor, bool isStatic) {
  if (widgetDrawn) {
    spdlog::error("Box already drawn. Not drawing at {} {}", startCol,
                  startRow);
    return uiError::UI_ALREADY_DRAWN;
  }
  this->isStatic = isStatic;
  int endCol = startCol + width;
  int endRow = startRow + height;
  if (endRow > windowHeight || endCol > windowWidth) {
    return uiError::DRAW_DIM_ERROR;
  }

  int innerWidth = width - 2;
  int innerHeight = height - 2;

  if (text.size() > innerWidth || height < 3) {
    spdlog::error(
        "Text is too big to be drawn inside the box. Text: {}, box width: {}",
        text.size(), innerWidth);
    return uiError::DRAW_DIM_ERROR;
  }

  int leftMargin = 0;
  if (centerAlign == true) {
    int availablePaddingWidth = innerWidth - text.size();
    leftMargin = availablePaddingWidth / 2;
  }

  int textRow = height / 2;

  int textLen = text.size();

  this->x = startCol;
  this->y = startRow;
  this->width = width;
  this->height = height;
  this->textStartRow = textRow;
  this->textStartCol = leftMargin;

  borderChars border = getBorderChars(shape);
  terminalManager.hideCursor();
  for (int i = 0; i < height; i++) {
    terminalManager.moveCursor(startRow + i, startCol);
    for (int j = 0; j < width; j++) {
      if (i == 0) { // Top edge
        if (j == 0) {
          terminalManager.writeToTerminal((char *)border.topLeft,
                                          strlen(border.topLeft));
        } else if (j == width - 1) {
          terminalManager.writeToTerminal((char *)border.topRight,
                                          strlen(border.topRight));
        } else {
          terminalManager.writeToTerminal((char *)border.horizontal,
                                          strlen(border.horizontal));
        }
      } else if (i == height - 1) { // Bottom edge
        if (j == 0) {
          terminalManager.writeToTerminal((char *)border.bottomLeft,
                                          strlen(border.bottomLeft));
        } else if (j == width - 1) {
          terminalManager.writeToTerminal((char *)border.bottomRight,
                                          strlen(border.bottomRight));
        } else {
          terminalManager.writeToTerminal((char *)border.horizontal,
                                          strlen(border.horizontal));
        }
      } else { // Middle rows
        if (j == 0 || j == width - 1) {
          terminalManager.writeToTerminal((char *)border.vertical,
                                          strlen(border.vertical));
        } else {
          if (i == textRow && j >= (1 + leftMargin) &&
              j < (1 + leftMargin + textLen)) {
            int textIndex = j - (1 + leftMargin);
            char ch = text[textIndex];

            terminalManager.writeToTerminal(&ch, 1);
          } else {
            terminalManager.writeToTerminal((char *)" ", 1);
          }
        }
      }
    }
  }
  widgetDrawn = true;
  isTextBox = true;
  terminalManager.showCursor();
  return uiError::OK;
}

std::vector<std::string> uiWidget::wrapText(std::string &text, int maxWidth) {
  std::vector<std::string> lines;
  std::istringstream stream(text);
  std::string currentLine;
  std::string word;
  while (stream >> word) {
    if (currentLine.empty()) {
      if (word.length() > maxWidth) {
        for (size_t i = 0; i < word.length(); i += maxWidth) {
          lines.push_back(word.substr(i, maxWidth));
        }
        continue;
      }
      currentLine = word;
    } else if (currentLine.length() + 1 + word.length() <= maxWidth) {
      currentLine + " " + word;
    } else {
      lines.push_back(currentLine);
      if (word.length() > maxWidth) {
        for (size_t i = 0; i < word.length(); i += maxWidth) {
          lines.push_back(word.substr(i, maxWidth));
        }
        currentLine.clear();
      } else {
        currentLine = word;
      }
    }
  }
  if (!currentLine.empty()) {
    lines.push_back(currentLine);
  }
  return lines;
}

borderChars uiWidget::getBorderChars(borderShape shape) {
  borderChars chars;
  switch (shape) {
  case borderShape::SHARP_SINGLE:
    chars.topLeft = BoxChars::TOP_LEFT_SHARP;
    chars.topRight = BoxChars::TOP_RIGHT_SHARP;
    chars.bottomLeft = BoxChars::BOTTOM_LEFT_SHARP;
    chars.bottomRight = BoxChars::BOTTOM_RIGHT_SHARP;
    chars.horizontal = BoxChars::HORIZONTAL;
    chars.vertical = BoxChars::VERTICAL;
    break;

  case borderShape::SHARP_DOUBLE:
    chars.topLeft = BoxChars::TOP_LEFT_DOUBLE;
    chars.topRight = BoxChars::TOP_RIGHT_DOUBLE;
    chars.bottomLeft = BoxChars::BOTTOM_LEFT_DOUBLE;
    chars.bottomRight = BoxChars::BOTTOM_RIGHT_DOUBLE;
    chars.horizontal = BoxChars::HORIZONTAL_DOUBLE;
    chars.vertical = BoxChars::VERTICAL_DOUBLE;
    break;

  case borderShape::ROUNDED_SINGLE:
    chars.topLeft = BoxChars::TOP_LEFT_ROUND;
    chars.topRight = BoxChars::TOP_RIGHT_ROUND;
    chars.bottomLeft = BoxChars::BOTTOM_LEFT_ROUND;
    chars.bottomRight = BoxChars::BOTTOM_RIGHT_ROUND;
    chars.horizontal = BoxChars::HORIZONTAL;
    chars.vertical = BoxChars::VERTICAL;
    break;

  case borderShape::ROUNDED_DOUBLE:
    chars.topLeft = BoxChars::TOP_LEFT_ROUND;
    chars.topRight = BoxChars::TOP_RIGHT_ROUND;
    chars.bottomLeft = BoxChars::BOTTOM_LEFT_ROUND;
    chars.bottomRight = BoxChars::BOTTOM_RIGHT_ROUND;
    chars.horizontal = BoxChars::HORIZONTAL_DOUBLE;
    chars.vertical = BoxChars::VERTICAL_DOUBLE;
    break;
  }

  return chars;
}
