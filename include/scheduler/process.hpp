//a template for the process
#include <iostream>
#include <string>
#include <cstdlib> // for rand() function

class Process {
private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;

public:
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
};

int main() {
    // Create a sample process
    Process myProcess("SampleProcess", 1, 10);

    // Execute instructions until the process finishes
    while (!myProcess.hasFinished()) {
        myProcess.executeInstruction();
    }

    std::cout << "Process has finished!\n";

    return 0;
}