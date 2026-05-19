#include "parser.hpp"

#include <sstream>
#include <string_view>

namespace {

bool readNext(std::istringstream &stream, std::string &token) {
  return static_cast<bool>(stream >> token);
}

bool hasNoMoreTokens(std::istringstream &stream) {
  std::string extra;
  return !(stream >> extra);
}

} // namespace

ParsedCommand Parser::parseCommand(std::string_view input) {
  ParsedCommand command;

  const std::string input_text(input);
  std::istringstream stream{input_text};
  std::string token;

  if (!readNext(stream, token)) {
    return command;
  }

  if (token == "exit") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::EXIT;
    }
    return command;
  }

  if (token == "initialize") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::INIT;
    }
    return command;
  }

  if (token == "screen") {
    if (!readNext(stream, token)) {
      return command;
    }

    command.type = ConsoleCommandType::SCREEN;

    if (token == "-ls") {
      command.screenType = ScreenCommandType::LIST;
      if (hasNoMoreTokens(stream)) {
        return command;
      }
      return ParsedCommand{};
    }

    if ((token == "-s" || token == "-r") && readNext(stream, command.target) &&
        hasNoMoreTokens(stream)) {
      command.screenType =
          token == "-s" ? ScreenCommandType::CREATE : ScreenCommandType::RESUME;
      return command;
    }

    return ParsedCommand{};
  }

  if (token == "process-smi") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::PROCESS_SMI;
    }

    return command;
  }

  if (token == "report-util") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::REPORT_UTIL;
    }

    return command;
  }

  if (token == "scheduler-start") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::SCHEDULER_START;
    }
    return command;
  }

  if (token == "scheduler-stop") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::SCHEDULER_STOP;
    }
    return command;
  }

  if (token == "clear") {
    if (hasNoMoreTokens(stream)) {
      command.type = ConsoleCommandType::CLEAR;
    }
    return command;
  }

  return ParsedCommand{};
}
