#include <args.hpp>
#include <stdexcept>
#include <string>

void args::parse(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--help" or arg == "-h") {
      help = true;
    } else if (arg == "--version" or arg == "-v") {
      version = true;
    } else if (arg == "--log" or arg == "-l") {
      log = true;
    } else if (arg == "--raylib-logs" or arg == "-r") {
      raylibLogs = true;
    } else {
      throw std::invalid_argument("Unknown argument: " + arg);
    }
  }
}
