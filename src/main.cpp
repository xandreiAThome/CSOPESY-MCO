#include "console.hpp"
#include "terminal_utils.hpp"
#include "globals.hpp"
#include <thread>

int main() {
  setConsoleOutputUtf8();

  std::thread tickThread([]() {
      while (Globals::get().running) {
          Globals::get().cpuCycles++;

          // update tick per frame (60 fps)
          std::this_thread::sleep_for(
              std::chrono::milliseconds(16)
          );
      }
  });

  tickThread.detach();

  Console console;
  console.run();

  // wait for threads to shut down
  std::this_thread::sleep_for(
      std::chrono::milliseconds(1000)
  );

  return 0;
}