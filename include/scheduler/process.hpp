//a template for the process
#pragma once
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
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions), processState(ProcessState::READY) {
    }

    // Execute one instruction of the process
    void executeInstruction() {
        if (remainingInstructions > 0) {
           // std::cout << "Core " << cpuCore << ": Executing instruction " <<
             //   totalInstructions - remainingInstructions + 1 << "/" << totalInstructions <<
               // " for " << name << "\n";

            commandList[totalInstructions - remainingInstructions]->execute();

            remainingInstructions--;
        }
        else {
            //std::cout << "Process " << id << ": " << name << " has already finished.\n";
        }
    }

    // Get the remaining number of instructions
    int getRemainingInstructions() const {
        return remainingInstructions;
    }

    int getExecutedInstructions() const {
        return totalInstructions - remainingInstructions;
    }

    int getTotalInstructions() const {
        return totalInstructions;
    }

    // Check if the process has finished
    bool hasFinished() const {
        return remainingInstructions == 0;
    }

    int getId() const {
        return id;
    }

    std::string getName() const {
        return name;
    }

    int getCpuCore() const {
        return cpuCore;
    }

    void addCommand(CommandType commandType) {
        switch (commandType) {
            case CommandType::PRINT:
                commandList.push_back(std::make_shared<PrintCommand>(id));
                break;
            default:
                std::cout << "Invalid command type.\n";
                break;
        }
    }

    void setState(ProcessState newState) {
        processState = newState;
    }

    void setCpuCore(int newCoreId) {
        this->cpuCore = newCoreId;
    }

    void setArrivalTime(unsigned long long newTime) {
        this->arrivalTime = newTime;
    }



private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;

    std::vector<std::shared_ptr<ICommand>> commandList;
    ProcessState processState;

    int cpuCore = -1;

    unsigned long long arrivalTime;
    


};

