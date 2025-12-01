#ifndef BOX_CHARS_H
#define BOX_CHARS_H

namespace BoxChars {
// Rounded corners
constexpr const char *TOP_LEFT_ROUND = "\u256D";
constexpr const char *TOP_RIGHT_ROUND = "\u256E";
constexpr const char *BOTTOM_LEFT_ROUND = "\u256F";
constexpr const char *BOTTOM_RIGHT_ROUND = "\u2570";

// Sharp corners
constexpr const char *TOP_LEFT_SHARP = "\u250C";
constexpr const char *TOP_RIGHT_SHARP = "\u2510";
constexpr const char *BOTTOM_LEFT_SHARP = "\u2514";
constexpr const char *BOTTOM_RIGHT_SHARP = "\u2518";

// Double lines
constexpr const char *TOP_LEFT_DOUBLE = "\u2554";
constexpr const char *TOP_RIGHT_DOUBLE = "\u2557";
constexpr const char *BOTTOM_LEFT_DOUBLE = "\u255A";
constexpr const char *BOTTOM_RIGHT_DOUBLE = "\u255D";

// Lines
constexpr const char *HORIZONTAL = "\u2500";
constexpr const char *VERTICAL = "\u2502";
constexpr const char *HORIZONTAL_DOUBLE = "\u2550";
constexpr const char *VERTICAL_DOUBLE = "\u2551";

// T-junctions (for dividers)
constexpr const char *T_LEFT = "\u251C";
constexpr const char *T_RIGHT = "\u2524";
constexpr const char *T_TOP = "\u252C";
constexpr const char *T_BOTTOM = "\u2534";
constexpr const char *CROSS = "\u253C";

// Progress bar blocks
constexpr const char *BLOCK_FULL = "\u2588";
constexpr const char *BLOCK_DARK = "\u2593";
constexpr const char *BLOCK_MEDIUM = "\u2592";
constexpr const char *BLOCK_LIGHT = "\u2591";
constexpr const char *BLOCK_UPPER_HALF = "\u2580";
constexpr const char *BLOCK_LOWER_HALF = "\u2584";
} // namespace BoxChars

#endif // BOX_CHARS_H
