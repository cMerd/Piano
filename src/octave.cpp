#include <octave.hpp>
#include <stdexcept>

octave::octave(int value) {
  if (value < 3 or value > 5) {
    throw std::range_error("Value out of range.");
  }
  val = value;
}

void octave::operator=(int value) {
  if (value < 3 or value > 5) {
    throw std::range_error("Value out of range.");
  }
  val = value;
}

octave::operator int() const { return val; }

int octave::toPercentage() const {
  switch (val) {
  case 3:
    return 0;
  case 4:
    return 50;
  case 5:
    return 100;
  default:
    throw std::logic_error("Invalid octave value.");
  }
}

void octave::setPercentage(int percent) {

  if (percent >= 0 and percent <= 33) {
    val = 3;
    return;
  }

  if (percent > 33 and percent <= 66) {
    val = 4;
    return;
  }

  if (percent > 66 and percent <= 100) {
    val = 5;
    return;
  }

  throw std::range_error("Value out of range");
}
