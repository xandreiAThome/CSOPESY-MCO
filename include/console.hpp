#pragma once
#include "console_state.hpp"
#include <memory>

class Console {
private:
  std::unique_ptr<ConsoleBaseState> current_state;

public:
  Console();

  void setState(std::unique_ptr<ConsoleBaseState> next);

  void run();

  const char *currentStateName() const;
};