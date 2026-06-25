//a template for the process
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "commands/icommand.hpp"

class Process {
public:
    enum ProcessState {
        READY,
        WAITING,
        RUNNING,
        FINISHED
    };

    Process(const std::string& processName, int processId, int numInstructions);

    void executeInstruction();
    int getRemainingInstructions() const;
    int getExecutedInstructions() const;
    int getTotalInstructions() const;
    bool hasFinished() const;
    int getId() const;
    std::string getName() const;
    int getCpuCore() const;
    uint16_t getVariable(const std::string& name);
    ProcessState getState() const;
    unsigned long long getWakeUpTick() const;
    const std::vector<std::string>& getLogs() const;

    void addCommand(std::shared_ptr<ICommand> command);
    void setState(ProcessState newState);
    void setCpuCore(int newCoreId);
    void setArrivalTime(unsigned long long newTime);
    void setVariable(const std::string& name, uint16_t value);
    void addLog(const std::string& logMessage);
    void sleep(uint8_t ticks);
    bool isReadyToWake() const;

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
    std::vector<std::string> logs;
    unsigned long long wakeUpTick = -1;
};

