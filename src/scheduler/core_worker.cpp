#include "scheduler/core_worker.hpp"
#include "scheduler/process.hpp"
#include "scheduler/global_scheduler.hpp"
#include <memory>
#include <thread>

CoreWorker::CoreWorker(int coreId) : coreId(coreId), isRunning(false) {}

void CoreWorker::run() {
    isRunning.store(true);
    while (isRunning.load()) {
        // wait idly until the dispatcher assigns a process
        if (currentProcess == nullptr) {
            std::this_thread::yield(); 
            continue;
        }

        executeProcess(currentProcess);
        
        // clear the current process so the dispatcher knows we are idle after finishing
        currentProcess = nullptr;
    }
}

void CoreWorker::executeProcess(std::shared_ptr<Process> process) {
    process->setState(Process::RUNNING);
    GlobalScheduler::get().markRunning(process);
    process->setCpuCore(coreId);
    
    int delay = Globals::get().delayPerExec;

    while (!process->hasFinished() && isRunning.load()) {
        process->executeInstruction();

        if (delay > 0) {
            waitForTicks(delay);
        }
    }

    if (process->hasFinished()) {
        process->setState(Process::FINISHED);
        process->setCpuCore(-2);

        GlobalScheduler::get().markFinished(process);
    }
}