#include "scheduler/process.hpp"
#include "globals.hpp"

Process::Process(const std::string& processName, int processId, int numInstructions)
    : name(processName), id(processId), totalInstructions(numInstructions),
    processState(static_cast<int>(ProcessState::READY)) {
}

void Process::executeInstruction() {
    if (!hasFinished()) {
        commandList[currentInstructionIndex.load()]->execute(*this);
        currentInstructionIndex.fetch_add(1);
    }
}

int Process::getRemainingInstructions() const {
    return totalInstructions - currentInstructionIndex.load();
}

int Process::getExecutedInstructions() const {
    return currentInstructionIndex.load();
}

bool Process::hasFinished() const {
    return currentInstructionIndex.load() >= totalInstructions;
}

int Process::getTotalInstructions() const {
    return totalInstructions;
}

int Process::getId() const {
    return id;
}

std::string Process::getName() const {
    return name;
}

int Process::getCpuCore() const {
    return cpuCore.load();
}

uint16_t Process::getVariable(const std::string& name) {
    if (variables.find(name) == variables.end()) {
        variables[name] = 0;
    }
    return variables[name];
}

Process::ProcessState Process::getState() const {
    return static_cast<ProcessState>(processState.load());
}

unsigned long long Process::getWakeUpTick() const {
    return wakeUpTick.load();
}

std::vector<std::string> Process::getLogs() const {
    std::lock_guard<std::mutex> lock(logsMutex);
    return logs;
}

void Process::addCommand(std::shared_ptr<ICommand> command) {
    commandList.push_back(command);
    totalInstructions = commandList.size();
}

void Process::setState(ProcessState newState) {
    processState.store(static_cast<int>(newState));
}

void Process::setCpuCore(int newCoreId) {
    cpuCore.store(newCoreId);
}

void Process::setArrivalTime(unsigned long long newTime) {
    this->arrivalTime = newTime;
}

void Process::setVariable(const std::string& name, uint16_t value) {
    variables[name] = value;
}

void Process::addLog(const std::string& logMessage) {
    std::lock_guard<std::mutex> lock(logsMutex);
    logs.push_back(logMessage);
}

void Process::sleep(uint8_t ticks) {
    wakeUpTick.store(Globals::get().cpuCycles + ticks);
    processState.store(static_cast<int>(ProcessState::WAITING));
}

bool Process::isReadyToWake() const {
    return static_cast<ProcessState>(processState.load()) == ProcessState::WAITING
        && Globals::get().cpuCycles >= wakeUpTick.load();
}
