//a template for the process
#include <iostream>
#include <string>
#include <cstdlib> 
#include <vector>
#include <memory>
#include "commands/icommand.hpp"
#include "commands/print_command.hpp"


class Process {
public:
    enum ProcessState {
        READY,
        WAITING,
        RUNNING,
        FINISHED
    };

    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions) {
    }

    // Execute one instruction of the process
    void executeInstruction() {
        if (remainingInstructions > 0) {
            std::cout << "Executing instruction for Process " << id << ": " << name << "\n";
            remainingInstructions--;
        }
        else {
            std::cout << "Process " << id << ": " << name << " has already finished.\n";
        }
    }

    // Get the remaining number of instructions
    int getRemainingInstructions() const {
        return remainingInstructions;
    }

    // Check if the process has finished
    bool hasFinished() const {
        return remainingInstructions == 0;
    }

    void addCommand(CommandType commandType) {
        switch (CommandType) {
            case CommandType::PRINT:
                commandList.push_back(std::make_shared<PrintCommand>(id));
                break;
            default:
                std::cout << "Invalid command type.\n";
                break;
        }
    }

private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;

    std::vector<std::shared_ptr<ICommand>> commandList;

    int cpuCoreId = -1;


};

