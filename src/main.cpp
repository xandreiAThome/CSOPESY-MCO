#include "console.hpp"
#include "terminal_utils.hpp"

int main() {
  setConsoleOutputUtf8();
  Console console;
  console.run();
  return 0;
}