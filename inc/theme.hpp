#pragma once

#include <map>
#include <raylib.h>
#include <string>

struct theme {
  Color backgroundColor;
  Color foregroundColor;
  Color sidebarColor;

  Color pianoKeyColor;
  Color pianoKeyHoverColor;
  Color pianoKeyClickColor;
  Color pianoKeyShadowColor;
  Color pianoKeyForegroundColor;
  Color pianoKeyOutlineColor;

  Color pianoSecondaryKeyColor;
  Color pianoSecondaryKeyHoverColor;
  Color pianoSecondaryKeyClickColor;
  Color pianoSecondaryKeyShadowColor;
  Color pianoSecondaryKeyForegroundColor;
  Color pianoSecondaryKeyOutlineColor;

  Color switchActiveColor;
  Color switchInactiveColor;
  Color switchColor;

  Color sliderActiveColor;
  Color sliderButtonColor;
  Color sliderDecorationColor;
  Color sliderInactiveColor;
  Color sliderClickColor;
  Color sliderHoverColor;

  Color buttonColor;
  Color buttonHoverColor;
  Color buttonClickColor;
  Color buttonOutlineColor;
};

namespace themes {
extern const theme defaultTheme;
extern const theme darkTheme;
extern const theme lightTheme;
} // namespace themes

extern std::map<std::string, theme> themeSelector;
