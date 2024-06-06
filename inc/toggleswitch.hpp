#pragma once

#include <raylib.h>

class toggleSwitch {
public:
  toggleSwitch();
  toggleSwitch(const Rectangle &rect, float radius, float animationSpeed,
               bool defaultValue, const Color &activeColor,
               const Color &inactiveColor, const Color &switchColor);

  bool getVal() const;
  void setVal(bool value);

  void draw();
  bool isClicked() const;

private:
  Rectangle box;
  float rad;
  float animSpeed;
  Color activeCol;
  Color inactiveCol;
  Color switchCol;
  bool val;
};
