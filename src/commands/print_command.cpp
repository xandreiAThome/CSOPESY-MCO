#include "commands/print_command.hpp"
#include "scheduler/process.hpp"

#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>

namespace {
  std::mutex localtimeMutex;
}

PrintCommand::PrintCommand()
    : message(""),
    variableName(""),
    hasVariable(false),
    useDefaultMessage(true) {
}

PrintCommand::PrintCommand(const std::string& message)
    : message(message),
    variableName(""),
    hasVariable(false),
    useDefaultMessage(false) {
}

PrintCommand::PrintCommand(const std::string& message, const std::string& variableName)
    : message(message),
    variableName(variableName),
    hasVariable(true),
    useDefaultMessage(false) {
}

void PrintCommand::execute(Process& process) {
    std::string outputMessage;

    if (useDefaultMessage) {
        outputMessage = "Hello world from " + process.getName() + "!";
    }
    else {
        outputMessage = message;

        if (hasVariable) {
            outputMessage += std::to_string(process.getVariable(variableName));
        }
    }

    auto t = std::time(nullptr);
    std::tm tm;

    {
        std::lock_guard<std::mutex> lock(localtimeMutex);
        tm = *std::localtime(&t);
    }

    std::ostringstream logLine;

    logLine << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p) ")
        << "Core: " << process.getCpuCore() << " "
        << "\"" << outputMessage << "\"";

    process.addLog(logLine.str());
}