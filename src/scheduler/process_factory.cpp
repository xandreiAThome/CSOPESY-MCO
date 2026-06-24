#include "scheduler/process_factory.hpp"

#include "commands/print_command.hpp"
#include "commands/declare_command.hpp"
#include "commands/add_command.hpp"
#include "commands/subtract_command.hpp"
#include "commands/sleep_command.hpp"
#include "commands/operand.hpp"

#include <cstdlib>

std::shared_ptr<Process> ProcessFactory::createProcess(
    const std::string& processName,
    int processId,
    int numInstructions
) {
    auto process = std::make_shared<Process>(processName, processId, 0);
    generateRandomCommands(process, numInstructions);
    return process;
}

std::shared_ptr<Process> ProcessFactory::createTestProcess(
    const std::string& processName,
    int processId
) {
    auto process = std::make_shared<Process>(processName, processId, 0);
    return process;
}

void ProcessFactory::generateRandomCommands(
    std::shared_ptr<Process> process,
    int numInstructions
) {
    // start generating blocks at For-Loop Depth 0
    auto commands = generateRandomCommandBlock(numInstructions, 0);
    
    for (const auto& cmd : commands) {
        process->addCommand(cmd);
    }
}

std::shared_ptr<ICommand> ProcessFactory::generateRandomBasicCommand() {
    int commandType = std::rand() % 5;
    std::string varName = randomVariableName();

    // helper to randomly decide between a variable or a raw value
    auto randomOp = []() -> Operand {
        if (std::rand() % 2 == 0) {
            return Operand::variable(randomVariableName());
        } else {
            return Operand::value(static_cast<uint16_t>(std::rand() % 100));
        }
    };

    switch (commandType) {
    case 0:
        // 50% chance to print a variable, 50% chance for default message
      //  if (std::rand() % 2 == 0) {
          //  return std::make_shared<PrintCommand>("Value from: ", varName);
        //} else {
            return std::make_shared<PrintCommand>(); 
       // }
    case 1:
        return std::make_shared<DeclareCommand>(varName, static_cast<uint16_t>(std::rand() % 100));
    case 2:
        return std::make_shared<AddCommand>(varName, randomOp(), randomOp());
    case 3:
        return std::make_shared<SubtractCommand>(varName, randomOp(), randomOp());
    case 4:
        return std::make_shared<SleepCommand>(static_cast<uint8_t>(1 + (std::rand() % 10)));
    default:
        return std::make_shared<PrintCommand>();
    }
}

std::vector<std::shared_ptr<ICommand>> ProcessFactory::generateRandomCommandBlock(
    int numInstructions,
    int currentForDepth
) {
    std::vector<std::shared_ptr<ICommand>> block;

    for (int i = 0; i < numInstructions; i++) {
        // 15% chance to spawn a for loop, only if we haven't hit the depth limit of 3
        if (currentForDepth < 3 && (std::rand() % 100 < 15)) {
            auto loopBlock = generateRandomForLoop(currentForDepth + 1);
            
            // immediately unroll the generated loop and append it to our current block
            block.insert(block.end(), loopBlock.begin(), loopBlock.end());
        } else {
            // otherwise, just push a standard instruction
            block.push_back(generateRandomBasicCommand());
        }
    }

    return block;
}

std::vector<std::shared_ptr<ICommand>> ProcessFactory::generateRandomForLoop(
    int currentForDepth
) {
    int innerInstructionsCount = 1 + (std::rand() % 3); 
    int repeats = 2 + (std::rand() % 4); 

    auto innerBlock = generateRandomCommandBlock(innerInstructionsCount, currentForDepth);

    return expandForLoop(innerBlock, repeats);
}

std::vector<std::shared_ptr<ICommand>> ProcessFactory::expandForLoop(
    const std::vector<std::shared_ptr<ICommand>>& instructions,
    int repeats
) {
    std::vector<std::shared_ptr<ICommand>> expanded;
    
    for (int i = 0; i < repeats; i++) {
        for (const auto& instruction : instructions) {
            expanded.push_back(instruction);
        }
    }
    return expanded;
}

std::string ProcessFactory::randomVariableName() {
    return "var_" + std::to_string(std::rand() % 10);
}