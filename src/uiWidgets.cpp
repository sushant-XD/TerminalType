#include "uiWidgets.h"
#include <spdlog/spdlog.h>
#include <sstream>

uiWidget::uiWidget(int windowWidth, int windowHeight, terminalCtrl &terminalMgr)
    : terminalManager(terminalMgr), windowWidth(windowWidth),
      windowHeight(windowHeight) {
  widgetDrawn = false;
  isStatic = false;
  isTextBox = false;
  initialPosCol = 0;
  initialPosRow = 0;
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
    spdlog::error("Given dimension don't fit inside the screen");
    return uiError::DRAW_DIM_ERROR;
  }
  borderChars border = getBorderChars(shape);

  this->initialPosCol = startCol;
  this->initialPosRow = startRow;
  this->width = width;
  this->height = height;
  this->textStartCol = -1;
  this->endRow = startRow + height;
  terminalManager.hideCursor();
  terminalManager.moveCursor(startRow, startCol);
  terminalManager.writeToTerminal(borderColor, strlen(borderColor));
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

uiError uiWidget::drawBoxWithText(int startCol, int startRow, int width,
                                  int height, std::string text,
                                  bool centerAlign, borderShape shape,
                                  char *borderColor, char *textColor,
                                  bool isStatic) {
  if (widgetDrawn) {
    spdlog::error("Box already drawn. Not drawing at {} {}", startCol,
                  startRow);
    return uiError::UI_ALREADY_DRAWN;
  }
  this->isStatic = isStatic;
  int endCol = startCol + width;
  int endRow = startRow + height;
  if (endRow > windowHeight || endCol > windowWidth) {
    spdlog::error("Dimensions os not compatible. End Rows and columns: {}{} "
                  "WindowDimensions: {}{}",
                  endRow, endCol, windowHeight, windowWidth);
    return uiError::DRAW_DIM_ERROR;
  }

  int innerWidth = width - 2;
  int innerHeight = height - 2;

  if (innerWidth <= 0 || innerHeight <= 0) {
    spdlog::error("Box too small for borders and text");
    return uiError::DRAW_DIM_ERROR;
  }

  // Wrap text to fit within the box
  std::vector<std::string> textLines;
  if (!text.empty()) {
    textLines = wrapText(text, innerWidth);

    if (textLines.size() > innerHeight) {
      spdlog::warn("Text too long, will be truncated to fit in box");
      textLines.resize(innerHeight);
    }
  }
  spdlog::info("Text Lines: {}", textLines[0]);

  this->initialPosCol = startCol;
  this->initialPosRow = startRow;
  this->width = width;
  this->height = height;
  this->textStartRow = startRow + 1;
  this->textStartCol = 2;
  this->endRow = startRow + height;
  this->initialText = text;

  borderChars border = getBorderChars(shape);
  terminalManager.hideCursor();
  for (int i = 0; i < height; i++) {
    terminalManager.moveCursor(startRow + i, startCol);
    spdlog::debug("Moving Cursor to: {}", startRow + i, startCol);
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
          // This is the interior space - print text if available
          int textLineIndex = i - 1; // Adjust for top border
          int textCharIndex = j - 1; // Adjust for left border
          spdlog::debug("TextlineIndex: {} textCharIndex: {} textLines Size: "
                        "{} textLine particular size",
                        textLineIndex, textCharIndex, textLines.size(),
                        textLines[textLineIndex].size());
          if (textLineIndex < textLines.size() &&
              !textLines[textLineIndex].empty()) {
            // Calculate padding for center alignment
            int padding = 0;
            if (centerAlign) {
              padding = (innerWidth - textLines[textLineIndex].length()) / 2;
            }

            if (textCharIndex >= padding &&
                textCharIndex < padding + textLines[textLineIndex].length()) {
              terminalManager.writeToTerminal(
                  &textLines[textLineIndex][textCharIndex - padding], 1);
              spdlog::debug("Printing {} at {}{}",
                            textLines[textLineIndex][textCharIndex - padding],
                            i, j);
            } else {
              spdlog::debug("Printing space at {}{}", i, j);
              terminalManager.writeToTerminal(" ", 1);
            }
          } else {
            terminalManager.writeToTerminal(" ", 1);
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

// refresh() refreshes the whole content (including the outer stuff)
uiError uiWidget::refresh() { return uiError::OK; }

// removes the element entirely
uiError uiWidget::erase() {
  char emptyChar = {' '};
  terminalManager.moveCursor(initialPosRow, initialPosCol);
  terminalManager.hideCursor();
  for (int i = initialPosRow; i < (initialPosRow + height); i++) {
    for (int j = initialPosCol; j < (initialPosCol + width); j++) {
      terminalManager.writeToTerminal(&emptyChar, 1);
    }
  }
  reset_vars();
  return uiError::OK;
}

void uiWidget::reset_vars() {
  spdlog::info("All variables reset for widget");
  return;
}

uiError uiWidget::updateText(char *ch, int startingIndexRow,
                             int startingIndexCol, int size, char *color) {
  if (isStatic) {
    spdlog::error("Cannot modify static Text");
    return uiError::STATIC_WIDGET;
  }
  if (!isTextBox) {
    spdlog::error("Is not a textBox. Cannot update Text.");
    return uiError::REFRESH_ERROR;
  }
  if (!widgetDrawn) {
    spdlog::error("Widget not drawn yet, cannot update Text");
    return uiError::REFRESH_ERROR;
  }
  if (ch == nullptr || size <= 0) {
    spdlog::error("Invalid parameters to function");
    return uiError::UNKNOWN_ERROR;
  }
  terminalManager.hideCursor();
  terminalManager.moveCursor(startingIndexRow, startingIndexCol);
  terminalManager.writeToTerminal(color, strlen(color));
  terminalManager.writeToTerminal(ch, size);
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
      currentLine += " " + word;
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
