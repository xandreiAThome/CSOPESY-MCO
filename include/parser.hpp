#pragma once

#include <string>
#include <string_view>

enum class ConsoleCommandType {
  EXIT,
  INIT,
  SCHEDULER_START,
  SCHEDULER_STOP,
  REPORT_UTIL,
  CLEAR,
  SCREEN,
  PROCESS_SMI,
  UNKNOWN,
};

enum class ScreenCommandType {
  LIST,
  CREATE,
  RESUME,
  UNKNOWN,
};

struct ParsedCommand {
  ConsoleCommandType type{ConsoleCommandType::UNKNOWN};
  ScreenCommandType screenType{ScreenCommandType::UNKNOWN};
  std::string target;
};

class Parser {
public:
  static ParsedCommand parseCommand(std::string_view input);
};
