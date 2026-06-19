#include "scheduler/core_worker.hpp"
#include "scheduler/process.hpp"
#include "scheduler/global_scheduler.hpp"
#include <memory>

CoreWorker::CoreWorker(int coreId) : coreId(coreId), isRunning(false) {}

void CoreWorker::run() {
    isRunning.store(true);
    while (isRunning.load()) {
        std::shared_ptr<Process> process = GlobalScheduler::get().getNextProcess(coreId);

        if (process == nullptr) {
            sleep(1);
            continue;
        }

        executeProcess(process);
    }
}

void CoreWorker::executeProcess(std::shared_ptr<Process> process) {
    // std::cout << "Core " << coreId + 1 << " running Process " << process->getId() << "\n";

    process->setState(Process::RUNNING);
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

        // std::cout << "Core " << coreId + 1 << " completed Process  << process->getId() << "\n";
    }
}