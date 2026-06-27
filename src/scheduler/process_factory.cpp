#include "scheduler/process_factory.hpp"

#include "commands/add_command.hpp"
#include "commands/declare_command.hpp"
#include "commands/operand.hpp"
#include "commands/print_command.hpp"
#include "commands/sleep_command.hpp"
#include "commands/subtract_command.hpp"
#include "commands/for_command.hpp"

#include <cstdlib>

std::shared_ptr<Process>
ProcessFactory::createProcess(const std::string &processName, int processId,
                              int numInstructions) {
  auto process = std::make_shared<Process>(processName, processId, 0);
  generateRandomCommands(process, numInstructions);
  return process;
}

std::shared_ptr<Process>
ProcessFactory::createTestProcess(const std::string &processName,
                                  int processId) {
  auto process = std::make_shared<Process>(processName, processId, 0);
  return process;
}

void ProcessFactory::generateRandomCommands(
    std::shared_ptr<Process> process,
    int numInstructions
) {
    std::vector<std::shared_ptr<ICommand>> finalCommands;

    while (static_cast<int>(finalCommands.size()) < numInstructions) {
        std::shared_ptr<ICommand> rawCommand;

        if (std::rand() % 100 < 15) {
            rawCommand = generateRandomForLoop(1);
        }
        else {
            rawCommand = generateRandomBasicCommand();
        }

        auto expandedCandidate = expandCommand(rawCommand);

        // Only insert for loop if it doesn't surpass the set number of instructions
        if (
            finalCommands.size() + expandedCandidate.size()
            <= static_cast<size_t>(numInstructions)
            ) {
            finalCommands.insert(
                finalCommands.end(),
                expandedCandidate.begin(),
                expandedCandidate.end()
            );
        }
        else {
            finalCommands.push_back(generateRandomBasicCommand());
        }
    }

    for (const auto& cmd : finalCommands) {
        process->addCommand(cmd);
    }
}

// Expand command if for loop. If not for loop, return same command
std::vector<std::shared_ptr<ICommand>>
ProcessFactory::expandCommand(
    const std::shared_ptr<ICommand>& command
) {
    std::vector<std::shared_ptr<ICommand>> expanded;

    auto forCommand = std::dynamic_pointer_cast<ForCommand>(command);

    if (forCommand) {
        forCommand->expandInto(expanded, 1);
    }
    else {
        expanded.push_back(command);
    }

    return expanded;
}

// Optional expand an already given vector of commands 
// e.g. [FOR(...), ADD, ADD, FOR(...)]
std::vector<std::shared_ptr<ICommand>>
ProcessFactory::expandCommands(
    const std::vector<std::shared_ptr<ICommand>>& commands
) {
    std::vector<std::shared_ptr<ICommand>> expanded;

    for (const auto& command : commands) {
        auto expandedCommand = expandCommand(command);

        expanded.insert(
            expanded.end(),
            expandedCommand.begin(),
            expandedCommand.end()
        );
    }

    return expanded;
}


std::shared_ptr<ICommand>
ProcessFactory::generateRandomBasicCommand() {
    int commandType = std::rand() % 5;  // Randmoly select non-for command type
    std::string varName = randomVariableName();

    // helper function for 50/50 variable or literal operand
    auto randomOp = []() -> Operand {
        if (std::rand() % 2 == 0) {
            return Operand::variable(randomVariableName());
        }
        return Operand::value(static_cast<uint16_t>(std::rand() % 100));
        };

    switch (commandType) {
    case 0:
        return std::make_shared<PrintCommand>();

    case 1:
        return std::make_shared<DeclareCommand>(
            varName,
            static_cast<uint16_t>(std::rand() % 100)
        );

    case 2:
        return std::make_shared<AddCommand>(
            varName,
            randomOp(),
            randomOp()
        );

    case 3:
        return std::make_shared<SubtractCommand>(
            varName,
            randomOp(),
            randomOp()
        );

    case 4:
        return std::make_shared<SleepCommand>(
            static_cast<uint8_t>(1 + (std::rand() % 10))
        );

    default:
        return std::make_shared<PrintCommand>();
    }
}

// Command block for FOR loop
std::vector<std::shared_ptr<ICommand>>
ProcessFactory::generateRandomCommandBlock(
    int numInstructions,
    int currentForDepth
) {
    const int MAX_DEPTH = 3;

    std::vector<std::shared_ptr<ICommand>> block;
    int remaining = numInstructions;

    while (remaining > 0) {
        if (currentForDepth < MAX_DEPTH && std::rand() % 100 < 15) {
            block.push_back(generateRandomForLoop(currentForDepth + 1));
        }
        else {
            block.push_back(generateRandomBasicCommand());
        }

        remaining--;
    }

    return block;
}

std::shared_ptr<ICommand>
ProcessFactory::generateRandomForLoop(int currentForDepth) {
    int innerInstructionsCount = 1 + (std::rand() % 3);
    int repeats = 2 + (std::rand() % 4);

    auto innerBlock = generateRandomCommandBlock(
        innerInstructionsCount,
        currentForDepth
    );

    return std::make_shared<ForCommand>(innerBlock, repeats);
}


std::string ProcessFactory::randomVariableName() {
  return "var_" + std::to_string(std::rand() % 10);
}