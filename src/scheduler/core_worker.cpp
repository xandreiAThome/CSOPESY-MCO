#include "scheduler/core_worker.hpp"
#include "scheduler/process.hpp"
#include "scheduler/global_scheduler.hpp"
#include "globals.hpp"
#include <memory>
#include <thread>

CoreWorker::CoreWorker(int coreId) : coreId(coreId) {}

void CoreWorker::run() {
    while (isRunning() && Globals::get().running) {
        std::shared_ptr<Process> process = GlobalScheduler::get().getNextProcess(coreId);

        // wait idly until process is assigned
        if (process == nullptr) {
            sleep(1);
            continue;
        }

        executeProcess(process);
        
       
    }
}

void CoreWorker::executeProcess(std::shared_ptr<Process> process) {
    GlobalScheduler::get().markRunning(process);
    process->setCpuCore(coreId);
    
    int delay = Globals::get().delayPerExec;

    while (!process->hasFinished() && isRunning() && Globals::get().running) {
        process->executeInstruction();

        if (process->getState() == Process::WAITING) {
            GlobalScheduler::get().markWaiting(process);
            return;
        }

        if (delay > 0) {
            waitForTicks(delay);
        }
    }

    if (process->hasFinished()) {
        process->setCpuCore(-2);
        GlobalScheduler::get().markFinished(process);
    }
}