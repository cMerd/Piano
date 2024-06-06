#include <iostream>

namespace console {

extern bool areLogsEnabled;

template <typename T> void log(const T &val) {
  if (areLogsEnabled)
    std::cout << "[ INFO ]: " << val << '\n';
}

template <typename T> void warn(const T &val) {
  if (areLogsEnabled)
    std::cout << "[ WARNING ]: " << val << '\n';
}

template <typename T> void error(const T &val) {
  if (areLogsEnabled)
    std::cout << "[ ERROR ]: " << val << '\n';
}

// prints even if DEBUG isn't true defined
template <typename T> void errorForce(const T &val) {
  std::cout << "[ ERROR ]: " << val << '\n';
}

} // namespace console
