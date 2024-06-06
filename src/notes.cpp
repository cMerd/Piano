#include <log.hpp>
#include <notes.hpp>
#include <stdexcept>

noteInfo::noteInfo(char note_, bool isSharp_, int strength_)
    : isSharp(isSharp_) {
  const std::string &notes_str = "abcdefg";
  if (notes_str.find(note_) == std::string::npos) {
    throw std::invalid_argument("Invalid note.");
  }
  this->note = note_;

  if (strength_ > 5) {
    throw std::invalid_argument("Invalid strength.");
  }

  this->strength = strength_;
}

noteInfo::noteInfo(const std::string &noteStr) {
  if (!isValidNote(noteStr)) {
    throw std::invalid_argument("Invalid note string.\n");
  }

  this->note = noteStr.front();
  this->strength = noteStr.back() - '0';
  this->isSharp = noteStr.size() == 3;
}

bool noteInfo::isValidNote(const std::string &noteStr) const noexcept {
  if (noteStr.size() != 2 and noteStr.size() != 3) {
    return false;
  }

  const std::string notes_str = "abcdefg";

  if (notes_str.find(noteStr.front()) == std::string::npos) {
    return false;
  }

  if (noteStr.size() == 3 and (!isdigit(noteStr.back()) or noteStr[1] != '#')) {
    return false;
  }

  if (noteStr.size() == 2 and (!isdigit(noteStr.back()))) {
    return false;
  }

  if (noteStr.back() > '5') {
    return false;
  }

  return true;
}

std::string noteInfo::formatNote(const std::string &noteStr) const noexcept {
  std::string result = "";
  for (const char &c : noteStr) {
    if (isalpha(c)) {
      result += tolower(c);
      continue;
    }
    if (isdigit(c)) {
      result += c;
      continue;
    }
    result += '#';
  }
  return result;
}

noteInfo::operator std::string() const noexcept {
  std::string result = "";
  result += note;
  if (isSharp) {
    result += '-';
  }
  result += strength + '0';

  return result;
}

// for std::map
bool noteInfo::operator<(const noteInfo &other) const noexcept { return false; }

notes::notes(const std::string &noteDirectory, const std::string &fileFormat)
    : dir(noteDirectory), format(fileFormat) {
  console::log("Constructed audio notes");
}

notes::~notes() {
  for (const auto &[info, sound] : noteSounds) {
    UnloadSound(sound);
  }
  console::log("Unloaded audio notes");
}

Sound notes::getNote(const noteInfo &note) {
  if (noteSounds.find(note) == noteSounds.end()) {
    const std::string audioFile = dir + (std::string)note + format;
    noteSounds[note] = LoadSound(audioFile.c_str());
    console::log("Loaded new note: " + (std::string)note);
  }
  return noteSounds[note];
}
