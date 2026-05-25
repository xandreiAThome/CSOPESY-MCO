
#include "console.hpp"
#include "parser.hpp"
#include "terminal_utils.hpp"
#include <iostream>
#include <memory>
#include <string>

Console::Console() : current_state(std::make_unique<UninitializedState>()) {
  if (current_state) {
    clearTerminal();
    std::cout << "Transitioned to: " << current_state->current_state() << '\n';
  }
}

void Console::setState(std::unique_ptr<ConsoleBaseState> next) {
  current_state = std::move(next);

  if (current_state) {
    std::cout << "Transitioned to: " << current_state->current_state() << '\n';
  }
}

const char *Console::currentStateName() const {
  return current_state->current_state();
}

void Console::run() {
  while (true) {
    std::cout << "\nCurrent state: " << currentStateName() << '\n';
    std::cout << current_state->commandPrompt() << '\n';
    std::cout << "> ";

    std::string choice;
    if (!std::getline(std::cin, choice)) {
      std::cout << "Invalid input. Exiting.\n";
      break;
    }

    const ParsedCommand command = Parser::parseCommand(choice);
    if (command.type == ConsoleCommandType::UNKNOWN) {
      std::cout << current_state->invalidCommandMessage() << '\n';
      continue;
    }

    if (command.type == ConsoleCommandType::EXIT) {
      if (dynamic_cast<ProcessScreenState *>(current_state.get()) != nullptr) {
        current_state->handle(*this, command);
        continue;
      }

      return;
    }

    if (command.type == ConsoleCommandType::CLEAR) {
      clearTerminal();
      continue;
    }

    if (!current_state->accepts(command)) {
      std::cout << current_state->invalidCommandMessage() << '\n';
      continue;
    }

    current_state->handle(*this, command);
  }
}