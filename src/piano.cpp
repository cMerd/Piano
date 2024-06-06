#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <log.hpp>
#include <octave.hpp>
#include <piano.hpp>
#include <theme.hpp>

notes piano::getAudios() {
  return notes(std::string(GetApplicationDirectory()) + "../assets/notes/",
               ".mp3");
}

void piano::disableRaylibLogs() {
  SetTraceLogLevel(LOG_NONE);
  console::log("Disabled raylib logs");
}

piano::piano() {
  SetTargetFPS(FPS);
  InitWindow(1500, 900, "Piano");
  InitAudioDevice();
  SetExitKey(0);
  console::log("Initialized window");
  getUIElements();
}

piano::~piano() {
  UnloadFont(UIElements.font);
  CloseAudioDevice();
  CloseWindow();
  console::log("Closed window");
}

bool piano::shouldClose() { return WindowShouldClose(); }

void piano::loadRecord(const std::string &filePath) {
  std::string fileContent = "", buffer;
  std::ifstream recordFile(filePath);

  if (!recordFile) {
    throw std::filesystem::filesystem_error("Couldn't open file: " + filePath,
                                            std::error_code());
  }

  while (std::getline(recordFile, buffer)) {
    fileContent += buffer;
    fileContent += '\n';
  }

  this->loadedRecord.contents = fileContent;
  this->loadedRecord.isValid = true;

  std::string cmd = "";
  do {
    cmd = "";
    for (size_t i = loadedRecord.index; i < loadedRecord.contents.size(); i++) {
      loadedRecord.index = i;
      if (loadedRecord.contents[i] == '\n') {
        break;
      }

      cmd += loadedRecord.contents[i];
    }
    loadedRecord.index++;
    if (!cmd.empty()) {
      loadedRecord.commands.push_back(cmd);
    }
  } while (!cmd.empty());

  loadedRecord.index = 0;
  console::log("Loaded record file: " + filePath);
  recordFile.close();
}

void piano::handleInput() {

  if (UIElements.isLocked) {
    int key = GetKeyPressed();
    if (key == KEY_ESCAPE) {
      UIElements.isLocked = false;
    } else if (key != 0) {
      UIElements.logMsg = {UIElements.colorTheme.foregroundColor,
                           "Press ESC to stop playing."};
      console::warn("Key press detected while playing a record.");
    }
    return;
  }

  if (IsFileDropped()) {
    FilePathList droppedFiles = LoadDroppedFiles();
    if (droppedFiles.count != 1) {
      UIElements.logMsg = {
          UIElements.colorTheme.foregroundColor,
          "Please drop only one file.",
      };
      console::error("Multiple files was dropped.");
    } else if (!IsFileExtension(droppedFiles.paths[0], ".piano")) {
      UIElements.logMsg = {
          UIElements.colorTheme.foregroundColor,
          "Only .piano files are supported.",
      };
      console::error("A file with unknown extension was dropped.");
    } else {
      clearLoadedRecord();
      loadRecord(droppedFiles.paths[0]);
    }

    UnloadDroppedFiles(droppedFiles);
    console::log("Unloaded dropped file(s).");
  }

  if (IsKeyPressed(KEY_THREE)) {
    UIElements.octaveValue = 3;
    UIElements.octaveSlider.setValue(UIElements.octaveValue.toPercentage());
    currentRecord.buffer << "octave " << (int)this->UIElements.octaveValue
                         << '\n';
    console::log("Octave set to 3");
  }

  if (IsKeyPressed(KEY_FOUR)) {
    UIElements.octaveValue = 4;
    UIElements.octaveSlider.setValue(UIElements.octaveValue.toPercentage());
    currentRecord.buffer << "octave " << (int)this->UIElements.octaveValue
                         << '\n';
    console::log("Octave set to 4");
  }

  if (IsKeyPressed(KEY_FIVE)) {
    UIElements.octaveValue = 5;
    UIElements.octaveSlider.setValue(UIElements.octaveValue.toPercentage());
    currentRecord.buffer << "octave " << (int)this->UIElements.octaveValue
                         << '\n';
    console::log("Octave set to 5");
  }

  if (IsKeyPressed(KEY_F1)) {
    UIElements.isHelpMenuVisible = !UIElements.isHelpMenuVisible;
    UIElements.helpMenuSwitch.setVal(UIElements.isHelpMenuVisible);
    console::log(std::string("Help menu is now ") +
                 (UIElements.isHelpMenuVisible ? "visible." : "hidden."));
  }
}

void piano::drawPiano() {
  bool applyAnimationOnly = false;
  if (UIElements.isLocked and
      loadedRecord.delayTime <= loadedRecord.frameCounter) {
    processCommand(loadedRecord.commands[loadedRecord.index++]);
    if (loadedRecord.index >= loadedRecord.commands.size()) {
      UIElements.isLocked = false;
      loadedRecord.index = 0;
      loadedRecord.delayTime = 0;
      loadedRecord.frameCounter = 0;
      loadedRecord.keyToPress = "";
      console::log("Stopped recording");
    }
  } else {
    if (UIElements.isLocked and
        loadedRecord.delayTime > loadedRecord.frameCounter) {
      applyAnimationOnly = true;
    }
    if (UIElements.isLocked and loadedRecord.frameCounter > 80) {
      loadedRecord.keyToPress = "";
    }
  }
  for (const auto &[key, text] : UIElements.keys) {
    if (UIElements.isLocked) {
      if (text == UIElements.notesToKeys[loadedRecord.keyToPress]) {
        key.renderActive();
        if (!applyAnimationOnly) {
          key.apply();
        }
      } else {
        key.renderIdle();
      }
    } else {
      key.render();
      key.checkForKeyPress()->join();
    }
    if (UIElements.isHelpMenuVisible) {
      DrawTextEx(UIElements.font, text.c_str(),
                 {key.getCenter().x - text.size() * 10,
                  key.getCenter().y - text.size() * 5},
                 25, 1, UIElements.colorTheme.pianoKeyForegroundColor);
    }
  }

  for (const auto &[key, text] : UIElements.secondaryKeys) {
    if (UIElements.isLocked) {
      if (text == UIElements.notesToKeys[loadedRecord.keyToPress]) {
        key.renderActive();
        if (!applyAnimationOnly) {
          key.apply();
        }
      } else {
        key.renderIdle();
      }
    } else {
      key.render();
    }
    if (UIElements.isHelpMenuVisible) {
      DrawTextEx(UIElements.font, text.c_str(),
                 {key.getCenter().x - text.size() * 10,
                  key.getCenter().y - text.size() * 5},
                 25, 1, UIElements.colorTheme.pianoSecondaryKeyForegroundColor);
    }
    key.checkForKeyPress()->join();
  }
}

void piano::clearLoadedRecord() {
  loadedRecord.index = 0;
  loadedRecord.contents = "";
  loadedRecord.commands = {};
  loadedRecord.isValid = false;
  loadedRecord.keyToPress = "";
  UIElements.isLocked = false;
  console::log("Record buffer is cleared.");
}

void piano::drawSidebar() {

  DrawRectangle(900, 0, 600, 1500, UIElements.colorTheme.sidebarColor);
  DrawTextEx(UIElements.font, "Help menu", {950, 50}, 25, 1,
             UIElements.colorTheme.foregroundColor);
  UIElements.helpMenuSwitch.draw();
  UIElements.isHelpMenuVisible = UIElements.helpMenuSwitch.getVal();

  DrawTextEx(
      UIElements.font,
      (std::string("Octave ") + std::to_string(UIElements.octaveValue)).c_str(),
      {950, 100}, 25, 1, UIElements.colorTheme.foregroundColor);
  UIElements.octaveSlider.render(UIElements.octaveSlider.getValue());

  for (const button &btn : UIElements.themeButtons) {
    btn.draw();
  }

  for (const auto &[btn, text] : UIElements.recordButtons) {
    btn.draw();
    if (text == "{state} Record") {
      DrawTextEx(UIElements.font,
                 (currentRecord.isRecording ? "Stop Record" : "Start Record"),
                 {btn.getCenter().x - text.size() * 5,
                  btn.getCenter().y - text.size()},
                 25, 1, UIElements.colorTheme.foregroundColor);
      continue;
    }
    if (text == "{load_state}") {
      DrawTextEx(UIElements.font, (UIElements.isLocked ? "Stop" : "Play"),
                 {btn.getCenter().x - text.size() * 2,
                  btn.getCenter().y - text.size()},
                 25, 1, UIElements.colorTheme.foregroundColor);
      continue;
    }
    DrawTextEx(UIElements.font, text.c_str(),
               {btn.getCenter().x - text.size() * 5,
                btn.getCenter().y - text.size() * 3},
               25, 1, UIElements.colorTheme.foregroundColor);
  }

  if (currentRecord.isRecording) {
    std::string logText =
        "Recording: " + std::to_string(getRecordingTime().count()) + " ms";
    DrawTextEx(UIElements.font, logText.c_str(), {10, 10}, 25, 1,
               UIElements.colorTheme.foregroundColor);
    DrawCircle(logText.size() * 14, 22, 10, RED);
  }

  if ((int)UIElements.logMsg.first.a > 0) {
    drawErrorText();
  }
}

void piano::drawErrorText() {
  static int cnt = 0;
  cnt++;
  if (cnt % 10 == 0 and UIElements.logMsg.first.a > 0)
    UIElements.logMsg.first.a--;

  DrawTextEx(UIElements.font, UIElements.logMsg.second.c_str(), {50, 50}, 25, 1,
             UIElements.logMsg.first);
}

std::chrono::milliseconds piano::getRecordingTime() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now() - currentRecord.startTime);
}

std::chrono::milliseconds piano::getTimeSinceLastPress() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now() - currentRecord.pastTime);
}

void piano::clearLastPressTime() {
  currentRecord.pastTime = std::chrono::system_clock::now();
}

void piano::getUIElements() {

  UIElements.colorThemeName = getTheme();
  if (UIElements.colorThemeName.empty()) {
    UIElements.colorThemeName = "default";
  }

  UIElements.colorTheme = themeSelector[UIElements.colorThemeName];

  UIElements.keys = {
      {button(
           {200, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'c', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "C\n";
             }
           },
           KEY_A),
       "A"},
      {button(
           {280, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'d', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "D\n";
             }
           },
           KEY_S),
       "S"},
      {button(
           {360, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'e', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "E\n";
             }
           },
           KEY_D),
       "D"},
      {button(
           {440, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'f', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "F\n";
             }
           },
           KEY_F),
       "F"},
      {button(
           {520, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'g', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "G\n";
             }
           },
           KEY_G),
       "G"},
      {button(
           {600, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'a', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "A\n";
             }
           },
           KEY_H),
       "H"},
      {button(
           {680, 300, 70, 250}, UIElements.colorTheme.pianoKeyColor,
           UIElements.colorTheme.pianoKeyOutlineColor,
           UIElements.colorTheme.pianoKeyHoverColor,
           UIElements.colorTheme.pianoKeyClickColor,
           UIElements.colorTheme.pianoKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'b', false, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "B\n";
             }
           },
           KEY_J),
       "J"}};

  UIElements.secondaryKeys = {
      {button(
           {250, 300, 40, 150}, UIElements.colorTheme.pianoSecondaryKeyColor,
           UIElements.colorTheme.pianoSecondaryKeyOutlineColor,
           UIElements.colorTheme.pianoSecondaryKeyHoverColor,
           UIElements.colorTheme.pianoSecondaryKeyClickColor,
           UIElements.colorTheme.pianoSecondaryKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'c', true, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "C#\n";
             }
           },
           KEY_W),
       "W"},
      {button(
           {330, 300, 40, 150}, UIElements.colorTheme.pianoSecondaryKeyColor,
           UIElements.colorTheme.pianoSecondaryKeyOutlineColor,
           UIElements.colorTheme.pianoSecondaryKeyHoverColor,
           UIElements.colorTheme.pianoSecondaryKeyClickColor,
           UIElements.colorTheme.pianoSecondaryKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'d', true, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "D#\n";
             }
           },
           KEY_E),
       "E"},
      {button(
           {490, 300, 40, 150}, UIElements.colorTheme.pianoSecondaryKeyColor,
           UIElements.colorTheme.pianoSecondaryKeyOutlineColor,
           UIElements.colorTheme.pianoSecondaryKeyHoverColor,
           UIElements.colorTheme.pianoSecondaryKeyClickColor,
           UIElements.colorTheme.pianoSecondaryKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'f', true, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "F#\n";
             }
           },
           KEY_T),
       "T"},
      {button(
           {570, 300, 40, 150}, UIElements.colorTheme.pianoSecondaryKeyColor,
           UIElements.colorTheme.pianoSecondaryKeyOutlineColor,
           UIElements.colorTheme.pianoSecondaryKeyHoverColor,
           UIElements.colorTheme.pianoSecondaryKeyClickColor,
           UIElements.colorTheme.pianoSecondaryKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'g', true, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "G#\n";
             }
           },
           KEY_Y),
       "Y"},
      {button(
           {650, 300, 40, 150}, UIElements.colorTheme.pianoSecondaryKeyColor,
           UIElements.colorTheme.pianoSecondaryKeyOutlineColor,
           UIElements.colorTheme.pianoSecondaryKeyHoverColor,
           UIElements.colorTheme.pianoSecondaryKeyClickColor,
           UIElements.colorTheme.pianoSecondaryKeyShadowColor, 10, 0,
           [this]() {
             PlaySound(this->UIElements.audios.getNote(
                 {'a', true, this->UIElements.octaveValue}));
             if (this->currentRecord.isRecording) {
               std::chrono::milliseconds timeSinceLastKeyPress =
                   getTimeSinceLastPress();
               if (timeSinceLastKeyPress.count() > 100) {
                 this->currentRecord.buffer
                     << "sleep " << timeSinceLastKeyPress.count() << '\n';
               }
               this->currentRecord.buffer << "A#\n";
             }
           },
           KEY_U),
       "U"}};

  UIElements.notesToKeys = {
      {"C", "A"},  {"D", "S"},  {"E", "D"},  {"F", "F"},
      {"G", "G"},  {"A", "H"},  {"B", "J"},  {"C#", "W"},
      {"D#", "E"}, {"F#", "T"}, {"G#", "Y"}, {"A#", "U"},
  };

  UIElements.octaveSlider = slider(
      UIElements.octaveValue.toPercentage(),
      [this](int val) { this->UIElements.octaveValue.setPercentage(val); },
      {1100, 100, 300, 30}, 0.2f, UIElements.colorTheme.sliderActiveColor,
      UIElements.colorTheme.sliderButtonColor,
      UIElements.colorTheme.sliderDecorationColor,
      UIElements.colorTheme.sliderInactiveColor,
      UIElements.colorTheme.sliderClickColor,
      UIElements.colorTheme.sliderHoverColor, 0.1f, 0.1f, 2, VERTICAL);

  UIElements.helpMenuSwitch =
      toggleSwitch({1080, 43, 80, 40}, 0.5f, 0.065f, 0,
                   UIElements.colorTheme.switchActiveColor,
                   UIElements.colorTheme.switchInactiveColor,
                   UIElements.colorTheme.switchColor);

  if (!UIElements.isLoaded) {

    UIElements.font =
        LoadFontEx((std::string(GetApplicationDirectory()) +
                    "./../assets/CodeNewRomanNerdFont-Regular.otf")
                       .c_str(),
                   25, NULL, 0);
    UIElements.audios = getAudios();
    UIElements.isLoaded = true;
  }

  UIElements.helpMenuSwitch.setVal(UIElements.isHelpMenuVisible);

  UIElements.themeButtons = {
      button({925, 800, 150, 60}, themeSelector["default"].backgroundColor,
             UIElements.colorTheme.sliderInactiveColor,
             themeSelector["default"].switchInactiveColor,
             themeSelector["default"].backgroundColor, {0, 0, 0, 0}, 2.0f, 0.5f,
             [this]() {
               UIElements.colorThemeName = "default";
               saveTheme();
               this->getUIElements();
             },
             {}),
      button({1125, 800, 150, 60}, themeSelector["dark theme"].backgroundColor,
             UIElements.colorTheme.sliderInactiveColor,
             themeSelector["dark theme"].switchInactiveColor,
             themeSelector["dark theme"].backgroundColor, {0, 0, 0, 0}, 2.0f,
             0.5f,
             [this]() {
               UIElements.colorThemeName = "dark theme";
               saveTheme();
               this->getUIElements();
             },
             {}),
      button({1325, 800, 150, 60}, themeSelector["light theme"].backgroundColor,
             UIElements.colorTheme.sliderInactiveColor,
             themeSelector["light theme"].switchInactiveColor,
             themeSelector["light theme"].backgroundColor, {0, 0, 0, 0}, 2.0f,
             0.5f,
             [this]() {
               UIElements.colorThemeName = "light theme";
               saveTheme();
               this->getUIElements();
             },
             {}),
  };

  UIElements.recordButtons = {
      {button({950, 150, 180, 70}, UIElements.colorTheme.buttonColor,
              UIElements.colorTheme.buttonOutlineColor,
              UIElements.colorTheme.buttonHoverColor,
              UIElements.colorTheme.buttonClickColor, {0, 0, 0, 0}, 0.2f, 0.1f,
              [this]() {
                this->currentRecord.isRecording =
                    !this->currentRecord.isRecording;
                if (this->currentRecord.isRecording) {
                  this->clearRecord();
                  console::log("Started recording.");
                  this->currentRecord.buffer
                      << "octave " << (int)this->UIElements.octaveValue << '\n';
                  return;
                }
                console::log("Stopped recording.");
              },
              {}),
       "{state} Record"},
      // "{state}" will be replaced by either "start" or "stop"

      {button(
           {950, 230, 85, 35}, UIElements.colorTheme.buttonColor,
           UIElements.colorTheme.buttonOutlineColor,
           UIElements.colorTheme.buttonHoverColor,
           UIElements.colorTheme.buttonClickColor, {0, 0, 0, 0}, 0.2f, 0.1f,
           [this]() {
             if (!this->currentRecord.isValid) {
               this->UIElements.logMsg = {
                   this->UIElements.colorTheme.foregroundColor,
                   "There's no available record.",
               };
               console::error("There's no available record to save.");
               return;
             }

             if (this->currentRecord.isRecording) {
               this->UIElements.logMsg = {
                   this->UIElements.colorTheme.foregroundColor,
                   "Stop recording to save it.",
               };
               console::error("Tried to save record, while it's not over yet.");
               return;
             }

             // implement save feature
             const std::string fileName =
                 std::format("{:%d-%m-%Y %H:%M:%OS}",
                             std::chrono::system_clock::now()) +
                 ".piano";

             const std::string recordDir =
                 std::string(GetApplicationDirectory()) + "../records/";

             if (!std::filesystem::exists(recordDir)) {
               std::filesystem::create_directories(recordDir);
             }

             this->saveRecord(recordDir + fileName);
             this->UIElements.logMsg = {
                 UIElements.colorTheme.foregroundColor,
                 "record saved as: \"" + fileName + "\" in record directory.",
             };
           },
           {}),
       "Save"},

      {button({1045, 230, 85, 35}, UIElements.colorTheme.buttonColor,
              UIElements.colorTheme.buttonOutlineColor,
              UIElements.colorTheme.buttonHoverColor,
              UIElements.colorTheme.buttonClickColor, {0, 0, 0, 0}, 0.2f, 0.1f,
              [this]() {
                if (!this->loadedRecord.isValid) {
                  this->UIElements.logMsg = {
                      UIElements.colorTheme.foregroundColor,
                      "Drag and drop a file first to play it.",
                  };
                  console::error(
                      "Tried to play record when there isn't one loaded.");
                  return;
                }

                this->UIElements.isLocked = true;
              },
              {}),
       "{load_state}"},
      // {load_state} will be replaced by either "Play" or "Stop"
  };

  console::log("Loaded UI elements.");
}

void piano::processCommand(const std::string &cmd) {
  if (cmd.find("octave") != std::string::npos) {
    UIElements.octaveValue = cmd.back() - '0';
    UIElements.octaveSlider.setValue(UIElements.octaveValue.toPercentage());
    console::log("Octave changed by command to " +
                 std::to_string((int)UIElements.octaveValue));
    return;
  }

  if (cmd.find("sleep") != std::string::npos) {
    uint64_t ms = std::stoull(cmd.substr(cmd.find(' ') + 1));
    loadedRecord.delayTime = msToFrame(ms);
    console::log("Delaying for " + std::to_string(loadedRecord.delayTime) +
                 " ms");
    return;
  }

  loadedRecord.keyToPress = cmd;
  console::log("Executing command: " + loadedRecord.keyToPress);
}

uint64_t piano::msToFrame(uint64_t ms) {
  // every frame is 1 / fps sec
  // every frame is 1000 / fps ms
  constexpr double ONE_FRAME_MS = (double)1000 / FPS;
  return ms / ONE_FRAME_MS;
}

void piano::update() {
  BeginDrawing();
  ClearBackground(UIElements.colorTheme.backgroundColor);

  if (UIElements.isLocked) {
    loadedRecord.frameCounter++;
  }

  handleInput();
  drawPiano();
  drawSidebar();

  EndDrawing();
}

void piano::clearRecord() {
  console::log("Current record buffer cleared.");
  this->currentRecord.buffer = std::stringstream("");
  this->currentRecord.pastTime = std::chrono::system_clock::now();
  this->currentRecord.startTime = std::chrono::system_clock::now();
  this->currentRecord.isValid = true;
}

void piano::saveRecord(const std::string &filePath) {
  std::ofstream outputFile(filePath);

  if (!outputFile) {
    throw std::filesystem::filesystem_error(
        std::string("Can't create nor open file: ") + filePath,
        std::error_code());
  }

  outputFile << currentRecord.buffer.str() << "sleep 100\n";
  console::log("Current record saved to: " + filePath);
  outputFile.close();
}

void piano::saveTheme() {
  std::ofstream themeFile(std::string(GetApplicationDirectory()) +
                          "./../data/theme");

  if (!themeFile) {
    throw std::filesystem::filesystem_error(
        "Couldn't open/create file data/theme.", std::error_code());
  }

  themeFile << UIElements.colorThemeName;
  console::log("Saved current theme: " + UIElements.colorThemeName);
  themeFile.close();
}

const std::string piano::getTheme() {
  std::ifstream themeFile(std::string(GetApplicationDirectory()) +
                          "./../data/theme");

  if (!themeFile) {
    std::ofstream themeFileOutput(std::string(GetApplicationDirectory()) +
                                  "./../data/theme");
    themeFileOutput << "default";
    themeFileOutput.close();
  }

  std::string returnValue;
  std::getline(themeFile, returnValue);
  themeFile.close();
  console::log("Theme found: " + returnValue);
  return returnValue;
}
