#include "console.hpp"
#include "parser.hpp"
#include "terminal_utils.hpp"
#include "globals.hpp"
#include "scheduler/global_scheduler.hpp"
#include "scheduler/process.hpp"
#include "scheduler/process_cache.hpp"
#include "commands/print_command.hpp"
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

// const char *ProcessScreenState::commandPrompt() const {
//   return "Choose command (process-smi, clear, exit):";
// }

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

  // start scheduler
  GlobalScheduler::get().start();


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
    GlobalScheduler::get().printProcessReport();
    return;
  }

  if (command.screenType == ScreenCommandType::CREATE) {
      if (!GlobalScheduler::get().processExists(command.target)) {
          GlobalScheduler::get().createProcess(command.target);

          clearTerminal();
          console.setState(std::make_unique<ProcessScreenState>(command.target));
          return;
      }
      else {
          std::cout << "Process name already exists\n";
      }
  }

  if (command.screenType == ScreenCommandType::RESUME) {

      if (GlobalScheduler::get().processExists(command.target)) {
          clearTerminal();
          console.setState(std::make_unique<ProcessScreenState>(command.target));
          return;
      }
      else {
          std::cout << "No process found: " << command.target << "\n";
      }
  }


if (command.type == ConsoleCommandType::SCHEDULER_START) {
      GlobalScheduler::get().toggleProcessGeneration(true);
      std::cout << "Process generation started.\n";
      return;
  }

  if (command.type == ConsoleCommandType::SCHEDULER_STOP) {
      GlobalScheduler::get().toggleProcessGeneration(false);
      std::cout << "Process generation stopped.\n";
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

void ProcessScreenState::handle(Console &console, const ParsedCommand &command) {
  if (command.type == ConsoleCommandType::EXIT) {
    clearTerminal();
    console.setState(std::make_unique<MainMenuState>());
    return;
  }

  if (command.type == ConsoleCommandType::PROCESS_SMI) {

      
      ProcessCache process;
      bool processFound = GlobalScheduler::get().getProcessCache(attachedProcessName, process);

      if (!processFound) {
          std::cout << "Process " << attachedProcessName << " not found.\n";
          return;
      }

      std::cout << "Process name: " << process.name << "\n";
      std::cout << "ID: " << process.pid << "\n\n";

      std::cout << "Logs:\n";
      int line = 0;
      for (const auto& log : process.logs) {
          std::cout << line << ":: " << log << "\n";
          line++;
      }

      std::cout << "\nCurrent instruction line: " << process.executedInstructions << "\n";
      std::cout << "Lines of code: " << process.totalInstructions << "\n";

      if (process.executedInstructions == process.totalInstructions) {
          std::cout << "Finished!\n\n";
      }
      
      return;
  }
}