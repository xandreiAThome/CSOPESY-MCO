#pragma once
#include "scheduler/ietthread.hpp"
#include "scheduler/process.hpp"
#include "globals.hpp"
#include <memory>
#include <atomic>

class CoreWorker : public IETThread {
public:
    CoreWorker(int coreId);
    virtual ~CoreWorker() = default;
private:
	int coreId;
	std::atomic<bool> isRunning;

    void run();
    void executeProcess(std::shared_ptr<Process> process);

    void waitForTicks(unsigned long long ticksToWait) {
        unsigned long long startTick = Globals::get().cpuCycles.load();

        while (
            isRunning.load() &&
            Globals::get().cpuCycles.load() - startTick < ticksToWait
            ) {
            sleep(1);
        }
    }
};