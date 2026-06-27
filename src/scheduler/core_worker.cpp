#include "scheduler/core_worker.hpp"
#include "globals.hpp"
#include "scheduler/global_scheduler.hpp"
#include "scheduler/process.hpp"
#include <memory>

CoreWorker::CoreWorker(int coreId) : coreId(coreId) {}

void CoreWorker::run() {
  while (isRunning() && Globals::get().running) {
    std::shared_ptr<Process> process =
        GlobalScheduler::get().getNextProcess(coreId);

    // wait idly until process is assigned
    if (process == nullptr) {
      sleep(1);
      continue;
    }

    executeProcess(process);
  }
}

void CoreWorker::waitForTicks(unsigned long long ticksToWait) {
  unsigned long long startTick = Globals::get().cpuCycles.load();

  while (isRunning() &&
         Globals::get().cpuCycles.load() - startTick < ticksToWait) {
    sleep(1);
  }
}

void CoreWorker::executeProcess(std::shared_ptr<Process> process) {
  GlobalScheduler::get().markRunning(process);
  process->setCpuCore(coreId);

  int delay = Globals::get().delayPerExec;
  bool isRR = Globals::get().scheduler == SchedulerType::RR;
  unsigned long long quantum = Globals::get().quantumCycles;
  unsigned long long burstStartTick = Globals::get().cpuCycles.load();

  while (!process->hasFinished() && isRunning() && Globals::get().running) {
    process->executeInstruction();

    if (process->getState() == Process::WAITING) {
      GlobalScheduler::get().markWaiting(process);
      return;
    }

    if (delay >= 0) {
        waitForTicks(delay + 1);  // Delay = 0 should execute an instruction per tick
    }

    // Check quantum cycles after waiting
    if (isRR) {
      unsigned long long elapsed = Globals::get().cpuCycles.load() - burstStartTick;
      if (elapsed >= quantum) {
        GlobalScheduler::get().preempt(process, coreId);
        return;
      }
    }

  }

  if (process->hasFinished()) {
    process->setCpuCore(-2);
    GlobalScheduler::get().markFinished(process);
  }
}