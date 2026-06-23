#pragma once

#include "commands/icommand.hpp"
#include <string>

class PrintCommand : public ICommand {
public:
    // PRINT("Hello world from <process_name>!")
    PrintCommand();

    // PRINT("some message")
    PrintCommand(const std::string& message);

    // PRINT("some message" + var)
    PrintCommand(const std::string& message, const std::string& variableName);

    void execute(Process& process) override;

private:
    std::string message;
    std::string variableName;

    bool hasVariable;
    bool useDefaultMessage;
};