#pragma once
#include "scheduler/ietthread.hpp"
#include "scheduler/process.hpp"
#include "globals.hpp"
#include <memory>
#include <atomic>
#include <thread>

class CoreWorker : public IETThread {
public:
    CoreWorker(int coreId);
    virtual ~CoreWorker() = default;

    // lets the dispatcher check if this core is free
    bool isIdle() const {
        return currentProcess == nullptr;
    }

    // lets the dispatcher hand this core a process
    void assignProcess(std::shared_ptr<Process> process) {
        currentProcess = process;
    }

private:
	int coreId;
	std::atomic<bool> isRunning;
    std::shared_ptr<Process> currentProcess = nullptr;

    void run();
    void executeProcess(std::shared_ptr<Process> process);

    void waitForTicks(unsigned long long ticksToWait) {
        unsigned long long startTick = Globals::get().cpuCycles.load();

        while (
            isRunning.load() &&
            Globals::get().cpuCycles.load() - startTick < ticksToWait) {
            std::this_thread::yield(); // prevents freezing of the OS when using sleep
        }
    }
};