#include "scheduler/core_worker.hpp"
#include "scheduler/process.hpp"
#include "scheduler/global_scheduler.hpp"
#include "globals.hpp"
#include <memory>
#include <thread>

CoreWorker::CoreWorker(int coreId) : coreId(coreId), isRunning(false) {}

void CoreWorker::run() {
    isRunning.store(true);
    while (isRunning.load() && Globals::get().running) {
        std::shared_ptr<Process> process = GlobalScheduler::get().getNextProcess(coreId);

        // wait idly until the dispatcher assigns a process
        if (process == nullptr) {
            sleep(1);
            continue;
        }

        executeProcess(process);
        
       
    }
}

void CoreWorker::executeProcess(std::shared_ptr<Process> process) {
    process->setState(Process::RUNNING);
    GlobalScheduler::get().markRunning(process);
    process->setCpuCore(coreId);
    
    int delay = Globals::get().delayPerExec;

    while (!process->hasFinished() && isRunning.load() && Globals::get().running) {
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