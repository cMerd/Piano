#pragma once

class octave {
public:
  octave(int value);
  void operator=(int value);
  operator int() const;
  int toPercentage() const;
  void setPercentage(int percent);

private:
  int val;
};
