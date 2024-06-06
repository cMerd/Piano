#pragma once

#include <button.hpp>
#include <chrono>
#include <notes.hpp>
#include <octave.hpp>
#include <slider.hpp>
#include <sstream>
#include <string>
#include <theme.hpp>
#include <toggleswitch.hpp>

class piano {
public:
  piano();
  ~piano();

  static bool shouldClose();
  static void disableRaylibLogs();
  void update();

private:
  struct UIData {
    theme colorTheme;
    notes audios;
    std::string colorThemeName;

    std::vector<std::pair<button, std::string>> keys;
    std::vector<std::pair<button, std::string>> secondaryKeys;
    std::map<std::string, std::string> notesToKeys;
    slider octaveSlider;
    toggleSwitch helpMenuSwitch;
    std::vector<button> themeButtons;
    std::vector<std::pair<button, std::string>> recordButtons;
    Font font;
    std::pair<Color, std::string> logMsg = {Color{0, 0, 0, 0}, ""};

    bool isLoaded = false;
    bool isHelpMenuVisible = false;
    bool isLocked = false;
    octave octaveValue = 4;
  };

  struct recording {
    std::stringstream buffer;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>
        pastTime;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>
        startTime;
    bool isRecording = false;
    bool isValid = false;
  };

  struct recordFile {
    size_t index = 0;
    std::string contents = "";
    std::vector<std::string> commands;
    bool isValid = false;
    std::string keyToPress = "";
    std::chrono::steady_clock::time_point pressStartTime;
    uint64_t delayTime = 0;
    uint64_t frameCounter = 0;
  };

private:
  void drawSidebar();
  void drawPiano();
  void drawErrorText();
  void handleInput();
  void getUIElements();

  void saveTheme();
  static const std::string getTheme();

  static notes getAudios();

  void clearRecord();
  void clearLoadedRecord();
  void saveRecord(const std::string &filePath);
  void loadRecord(const std::string &filePath);
  std::chrono::milliseconds getRecordingTime() const;
  std::chrono::milliseconds getTimeSinceLastPress() const;
  void clearLastPressTime();

  void processCommand(const std::string &cmd);
  uint64_t msToFrame(uint64_t ms);

private:
  UIData UIElements;
  recording currentRecord;
  recordFile loadedRecord;

private:
  constexpr static int FPS = 60;
  constexpr static int FONT_SIZE = 25;
};
