#pragma once
#include "scheduler/ietthread.hpp"
#include "scheduler/process.hpp"
#include <memory>

class CoreWorker : public IETThread {
public:
  CoreWorker(int coreId);
  virtual ~CoreWorker() = default;

private:
  int coreId;

  void run();
  void executeProcess(std::shared_ptr<Process> process);

  void waitForTicks(unsigned long long ticksToWait);
};