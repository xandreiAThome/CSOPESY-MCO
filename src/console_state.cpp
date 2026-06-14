#include "console.hpp"
#include "parser.hpp"
#include "terminal_utils.hpp"
#include "globals.hpp"
#include "scheduler/fcfsscheduler.hpp"
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
    bool configSuccess = Globals::get().init();
    clearTerminal();
    
    if (configSuccess) {
        console.setState(std::make_unique<MainMenuState>());
        std::cout << "Settings initialized:\n";
        std::cout << "Number of CPUs: " << Globals::get().numCpu << '\n';
        std::cout << "Scheduler: "
            << (Globals::get().scheduler == SchedulerType::FCFS ? "FCFS" : "RR")
            << '\n';
        std::cout << "Quantum Cycles: " << Globals::get().quantumCycles << '\n';
        std::cout << "Batch Process Frequency: " << Globals::get().batchProcessFreq << '\n';
        std::cout << "Minimum Instructions: " << Globals::get().minIns << '\n';
        std::cout << "Maximum Instructions: " << Globals::get().maxIns << '\n';
        std::cout << "Delay Per Execution: " << Globals::get().delayPerExec << '\n';
    }
    else {
        std::cout << "Errors found in config.txt \n";
    }
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
    std::cout << "screen -ls command recognized. Doing something.\n";
    return;
  }

  if (command.screenType == ScreenCommandType::CREATE ||
      command.screenType == ScreenCommandType::RESUME) {
    clearTerminal();
    console.setState(std::make_unique<ProcessScreenState>());
    return;
  }

  if (command.type == ConsoleCommandType::SCHEDULER_START) {
      FCFSScheduler::get().schedulerStart();
      FCFSScheduler::get().runScheduler();
      return;
  }

  if (command.type == ConsoleCommandType::SCHEDULER_STOP) {
      FCFSScheduler::get().schedulerStop();
    return;
  }

  if (command.type == ConsoleCommandType::REPORT_UTIL) {
    std::cout << "report-util command recognized. Doing something.\n";
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
    console.setState(std::make_unique<MainMenuState>());
    return;
  }

  if (command.type == ConsoleCommandType::PROCESS_SMI) {
    std::cout << "process-smi command recognized. Doing something.\n";
    return;
  }
}