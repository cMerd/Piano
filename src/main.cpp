#define PIANO_VERSION "v0.1.0"

#include <args.hpp>
#include <iostream>
#include <log.hpp>
#include <piano.hpp>
#include <stdexcept>

int main(int argc, char *argv[]) {

  args arguments;

  try {
    arguments.parse(argc, argv);
  } catch (const std::invalid_argument &err) {
    std::cerr << err.what() << '\n';
    return 1;
  }

  if (arguments.help) {
    std::cout << "Piano - a simple virtual piano app\n";
    std::cout << "Arguments:\n";
    std::cout << "\t--help, -h: show this help page\n";
    std::cout << "\t--version, -v: print version\n";
    std::cout << "\t--log, -l: output debug logs\n";
    std::cout << "\t--raylib-logs, -r: output raylib logs\n";
    return 0;
  }

  if (arguments.version) {
    std::cout << "Piano version: " << PIANO_VERSION << '\n';
    return 0;
  }

  console::areLogsEnabled = arguments.log;
  if (!arguments.raylibLogs) {
    piano::disableRaylibLogs();
  }

  piano app;
  while (!app.shouldClose()) {
    app.update();
  }

  return 0;
}
