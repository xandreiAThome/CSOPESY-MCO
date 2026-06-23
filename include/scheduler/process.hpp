//a template for the process
#pragma once
#include <iostream>
#include <string>
#include <cstdlib> 
#include <vector>
#include <memory>
#include <unordered_map>
#include "commands/icommand.hpp"
#include "commands/print_command.hpp"
#include "globals.hpp"


class Process {
public:
    enum ProcessState {
        READY,
        WAITING,
        RUNNING,
        FINISHED
    };

    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), 
        processState(ProcessState::READY) {
    }

    // Execute one instruction of the process
    void executeInstruction() {
        if (!hasFinished()) {
           // std::cout << "Core " << cpuCore << ": Executing instruction " <<
             //   totalInstructions - remainingInstructions + 1 << "/" << totalInstructions <<
               // " for " << name << "\n";

            commandList[currentInstructionIndex]->execute(*this);
            currentInstructionIndex++;
        }
        else {
            //std::cout << "Process " << id << ": " << name << " has already finished.\n";
            return;
        }
    }

    // Get the remaining number of instructions
    int getRemainingInstructions() const {
        return totalInstructions - currentInstructionIndex;
    }

    int getExecutedInstructions() const {
        return currentInstructionIndex;
    }

    int getTotalInstructions() const {
        return totalInstructions;
    }

    // Check if the process has finished
    bool hasFinished() const {
        return currentInstructionIndex >= totalInstructions;
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

    uint16_t getVariable(const std::string& name) {
        if (variables.find(name) == variables.end()) {
            variables[name] = 0;
        }

        return variables[name];
    }

    ProcessState getState() const {
        return processState;
    }
    
    // if asleep
    unsigned long long getWakeUpTick() const {
        return wakeUpTick;
    }

    const std::vector<std::string>& getLogs() const {
        return logs;
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


    void setVariable(const std::string& name, uint16_t value) {
        variables[name] = value;
    }

    void addLog(const std::string& logMessage) {
        logs.push_back(logMessage);
    }


    void sleep(uint8_t ticks) {
        wakeUpTick = Globals::get().cpuCycles + ticks;
        processState = ProcessState::WAITING;
    }

    // if asleep
    bool isReadyToWake() const {
        return processState == ProcessState::WAITING && Globals::get().cpuCycles >= wakeUpTick;
    }

   

    

private:
    std::string name;
    int id;
    int totalInstructions;
    int currentInstructionIndex = 0;

    std::vector<std::shared_ptr<ICommand>> commandList;
    ProcessState processState;

    int cpuCore = -1;

    unsigned long long arrivalTime;

    std::unordered_map<std::string, uint16_t> variables;

    std::vector<std::string> logs;  // for process-smi

    unsigned long long wakeUpTick = -1 // if asleep
    
};

