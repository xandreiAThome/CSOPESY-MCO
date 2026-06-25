#include "scheduler/process.hpp"
#include "globals.hpp"

Process::Process(const std::string& processName, int processId, int numInstructions)
    : name(processName), id(processId), totalInstructions(numInstructions),
    processState(ProcessState::READY) {
}

void Process::executeInstruction() {
    if (!hasFinished()) {
        commandList[currentInstructionIndex]->execute(*this);
        currentInstructionIndex++;
    }
}

int Process::getRemainingInstructions() const {
    return totalInstructions - currentInstructionIndex;
}

int Process::getExecutedInstructions() const {
    return currentInstructionIndex;
}

int Process::getTotalInstructions() const {
    return totalInstructions;
}

bool Process::hasFinished() const {
    return currentInstructionIndex >= totalInstructions;
}

int Process::getId() const {
    return id;
}

std::string Process::getName() const {
    return name;
}

int Process::getCpuCore() const {
    return cpuCore;
}

uint16_t Process::getVariable(const std::string& name) {
    if (variables.find(name) == variables.end()) {
        variables[name] = 0;
    }
    return variables[name];
}

Process::ProcessState Process::getState() const {
    return processState;
}

unsigned long long Process::getWakeUpTick() const {
    return wakeUpTick;
}

const std::vector<std::string>& Process::getLogs() const {
    return logs;
}

void Process::addCommand(std::shared_ptr<ICommand> command) {
    commandList.push_back(command);
    totalInstructions = commandList.size();
}

void Process::setState(ProcessState newState) {
    processState = newState;
}

void Process::setCpuCore(int newCoreId) {
    this->cpuCore = newCoreId;
}

void Process::setArrivalTime(unsigned long long newTime) {
    this->arrivalTime = newTime;
}

void Process::setVariable(const std::string& name, uint16_t value) {
    variables[name] = value;
}

void Process::addLog(const std::string& logMessage) {
    logs.push_back(logMessage);
}

void Process::sleep(uint8_t ticks) {
    wakeUpTick = Globals::get().cpuCycles + ticks;
    processState = ProcessState::WAITING;
}

bool Process::isReadyToWake() const {
    return processState == ProcessState::WAITING && Globals::get().cpuCycles >= wakeUpTick;
}
