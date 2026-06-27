#pragma once

#include "process.hpp"
#include "commands/icommand.hpp"

#include <memory>
#include <string>
#include <vector>


class ProcessFactory {
public:
    static std::shared_ptr<Process> createProcess(
        const std::string& processName,
        int processId,
        int numInstructions
    );

    static std::shared_ptr<Process> createTestProcess(
        const std::string& processName,
        int processId
    );

    // Expand for command
    static std::vector<std::shared_ptr<ICommand>> expandCommand(
        const std::shared_ptr<ICommand>& command
    );

    static std::vector<std::shared_ptr<ICommand>> expandCommands(
        const std::vector<std::shared_ptr<ICommand>>& commands
    );

    static std::shared_ptr<Process> createFixedProcess(
        const std::string& processName,
        int processId
    );
private:
    static void generateRandomCommands(
        std::shared_ptr<Process> process,
        int numInstructions
    );

    static std::vector<std::shared_ptr<ICommand>> generateRandomCommandBlock(
        int numInstructions,
        int currentForDepth
    );

    static std::shared_ptr<ICommand> generateRandomBasicCommand();

    static std::shared_ptr<ICommand> generateRandomForLoop(
        int currentForDepth
    );

    

    static std::string randomVariableName();
};