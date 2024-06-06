#pragma once

#include <map>
#include <raylib.h>
#include <string>

class noteInfo {
public:
  noteInfo(char note_, bool isSharp_, int strength_);
  noteInfo(const std::string &noteStr);
  operator std::string() const noexcept;
  bool operator<(const noteInfo &other) const noexcept; // for std::map

private:
  std::string formatNote(const std::string &noteStr) const noexcept;
  bool isValidNote(const std::string &noteStr) const noexcept;

  char note;
  bool isSharp;
  int strength;
};

class notes {
public:
  notes() = default;
  notes(const std::string &noteDirectory, const std::string &fileFormat);

  ~notes();

  Sound getNote(const noteInfo &note);

private:
  std::string dir, format;
  std::map<std::string, Sound> noteSounds;
};
