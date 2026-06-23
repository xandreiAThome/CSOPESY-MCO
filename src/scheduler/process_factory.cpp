#include "scheduler/process_factory.hpp"

#include "commands/print_command.hpp"
#include "commands/declare_command.hpp"
#include "commands/add_command.hpp"
#include "commands/subtract_command.hpp"
#include "commands/sleep_command.hpp"
#include "commands/operand.hpp"

#include <cstdlib>
#include <memory>
#include <string>

std::shared_ptr<Process> ProcessFactory::createProcess(int pid) {
    std::string processName;

    if (pid < 10) {
        processName = "screen_0" + std::to_string(pid);
    }
    else {
        processName = "screen_" + std::to_string(pid);
    }

    int numInstructions = 5 + (std::rand() % 16);
    // example: 5 to 20 instructions

    auto process = std::make_shared<Process>(
        processName,
        pid,
        0
    );

    generateInstructions(*process);

    return process;
}

void ProcessFactory::generateInstructions(Process& process) {
    int instructionCount = 5 + (std::rand() % 16);

    for (int i = 0; i < instructionCount; i++) {
        int commandType = std::rand() % 5;

        switch (commandType) {
        case 0:
            process.addCommand(std::make_shared<PrintCommand>());
            break;

        case 1:
            process.addCommand(std::make_shared<DeclareCommand>(
                "x",
                static_cast<uint16_t>(std::rand() % 100)
            ));
            break;

        case 2:
            process.addCommand(std::make_shared<AddCommand>(
                "x",
                Operand::variable("x"),
                Operand::value(static_cast<uint16_t>(std::rand() % 100))
            ));
            break;

        case 3:
            process.addCommand(std::make_shared<SubtractCommand>(
                "x",
                Operand::variable("x"),
                Operand::value(static_cast<uint16_t>(std::rand() % 100))
            ));
            break;

        case 4:
            process.addCommand(std::make_shared<SleepCommand>(
                static_cast<uint8_t>(1 + (std::rand() % 10))
            ));
            break;
        }
    }
}

std::vector<std::shared_ptr<ICommand>> expandForLoop(
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