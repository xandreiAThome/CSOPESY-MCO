#pragma once

#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "scheduler/core_worker.hpp"
#include "scheduler/ietthread.hpp"
#include "scheduler/ischeduler.hpp"
#include "scheduler/process.hpp"
#include "scheduler/process_cache.hpp"

class GlobalScheduler : public IETThread {
public:
  GlobalScheduler(int cores);

  static GlobalScheduler &get();

  void start();
  void toggleProcessGeneration(bool enable);
  void addProcess(std::shared_ptr<Process> process);
  std::shared_ptr<Process> getNextProcess(int coreId);
  bool getProcessCache(const std::string &name, ProcessCache &outCache);
  void markWaiting(std::shared_ptr<Process> process);
  void markRunning(std::shared_ptr<Process> process);
  void markFinished(std::shared_ptr<Process> process);
  std::string buildProcessReport();
  void printProcessReport();
  void writeProcessReportToFile();
  bool processExists(const std::string &pName);
  void createProcess(const std::string &name);
  void wakeReadyProcesses();
  void stop();
  bool allWorkersFinished() const;

private:
  std::map<std::string, std::shared_ptr<Process>> readyProcesses;
  std::map<std::string, std::shared_ptr<Process>> waitingProcesses;
  std::map<std::string, std::shared_ptr<Process>> runningProcesses;
  std::map<std::string, ProcessCache> finishedProcessCache;

  std::mutex mutex;

  std::vector<std::unique_ptr<CoreWorker>> workers;
  std::unique_ptr<IScheduler> scheduler;

  int numCores;
  int nextCore = 0;

  std::atomic<bool> generateProcesses{false};
  std::atomic<int> generatedProcessCount{0};

  void run() override;
  void writeProcessInfo(std::ostream &out,
                        const std::shared_ptr<Process> &process);
  void writeCpuStatus(std::ostream &out);
  void writeFinishedProcessInfo(std::ostream &out, const ProcessCache &process);
  ProcessCache createCacheFromProcess(const std::shared_ptr<Process> &process);
  bool processExistsNoLock(const std::string &pName);
  int generatePid();
};