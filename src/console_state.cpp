#include "console.hpp"
#include "parser.hpp"
#include "terminal_utils.hpp"
#include <iostream>
#include <memory>

const char *UninitializedState::commandPrompt() const {
  return "Choose command (initialize, exit, clear):";
}

const char *UninitializedState::invalidCommandMessage() const {
  return "Unknown command. Type initialize to start.";
}

const char *MainMenuState::commandPrompt() const {
  return "Choose command (screen -ls, screen -s <name>, screen -r <name>, "
         "scheduler-start, scheduler-stop, report-util, clear, exit):";
}

const char *MainMenuState::invalidCommandMessage() const {
  return "Unknown command. Try again.";
}

const char *ProcessScreenState::commandPrompt() const {
  return "Choose command (process-smi, clear, exit):";
}

const char *ProcessScreenState::invalidCommandMessage() const {
  return "Unknown command. Try again.";
}

bool UninitializedState::accepts(const ParsedCommand &command) const {
  return command.type == ConsoleCommandType::INIT;
}

void UninitializedState::handle(Console &console,
                                const ParsedCommand &command) {
  if (command.type == ConsoleCommandType::INIT) {
    clearTerminal();
    printTextFile("assets/introText.txt");
    console.setState(std::make_unique<MainMenuState>());
  }
}

bool MainMenuState::accepts(const ParsedCommand &command) const {
  return (command.type == ConsoleCommandType::SCREEN &&
          (command.screenType == ScreenCommandType::LIST ||
           command.screenType == ScreenCommandType::CREATE ||
           command.screenType == ScreenCommandType::RESUME)) ||
         command.type == ConsoleCommandType::SCHEDULER_START ||
         command.type == ConsoleCommandType::SCHEDULER_STOP ||
         command.type == ConsoleCommandType::REPORT_UTIL;
}

void MainMenuState::handle(Console &console, const ParsedCommand &command) {
  if (command.screenType == ScreenCommandType::LIST) {
    std::cout << "screen -ls command recognized\n";
    return;
  }

  if (command.screenType == ScreenCommandType::CREATE ||
      command.screenType == ScreenCommandType::RESUME) {
    clearTerminal();
    console.setState(std::make_unique<ProcessScreenState>());
    return;
  }

  if (command.type == ConsoleCommandType::SCHEDULER_START) {
    std::cout << "scheduler-start command recognized\n";
    return;
  }

  if (command.type == ConsoleCommandType::SCHEDULER_STOP) {
    std::cout << "scheduler-stop command recognized\n";
    return;
  }

  if (command.type == ConsoleCommandType::REPORT_UTIL) {
    std::cout << "report-util command recognized\n";
    return;
  }
}

bool ProcessScreenState::accepts(const ParsedCommand &command) const {
  return command.type == ConsoleCommandType::EXIT ||
         command.type == ConsoleCommandType::PROCESS_SMI;
}

void ProcessScreenState::handle(Console &console,
                                const ParsedCommand &command) {
  if (command.type == ConsoleCommandType::EXIT) {
    clearTerminal();
    printTextFile("assets/introText.txt");
    console.setState(std::make_unique<MainMenuState>());
    return;
  }

  if (command.type == ConsoleCommandType::PROCESS_SMI) {
    std::cout << "process-smi command recognized\n";
    return;
  }
}