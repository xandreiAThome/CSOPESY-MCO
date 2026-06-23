#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <map>
#include <atomic>
#include <thread>
#include <string>

#include "scheduler/process.hpp"
#include "scheduler/ietthread.hpp"
#include "scheduler/fcfsscheduler.hpp"
#include "scheduler/core_worker.hpp"
#include "scheduler/process_cache.hpp"
#include "scheduler/process_factory.hpp"
#include "globals.hpp"

class GlobalScheduler : public IETThread {
public:
    GlobalScheduler(int cores)
        : scheduler(std::make_unique<FCFSScheduler>(cores)),
        numCores(cores)
    {
        for (int i = 0; i < numCores; i++) {
            workers.push_back(std::make_unique<CoreWorker>(i));
        }
    }

    static GlobalScheduler& get() {
        static GlobalScheduler instance(Globals::get().numCpu);
        return instance;
    }

    void start() {
        for (auto& worker : workers) {
            worker->start();
        }
        IETThread::start(); 
    }


    void toggleProcessGeneration(bool enable) {
        generateProcesses.store(enable);
    }

    void addProcess(std::shared_ptr<Process> process) {
        int assignedCore;

        {
            std::lock_guard<std::mutex> lock(mutex);
           // processes[process->getId()] = process;

            
            assignedCore = nextCore;          
            nextCore = (nextCore + 1) % numCores;
        }

        process->setArrivalTime(Globals::get().cpuCycles);
        scheduler->enqueue(process, assignedCore);
    }

    /*
    std::shared_ptr<Process> getProcess(int pid) { 
        std::lock_guard<std::mutex> lock(mutex); 
        auto it = processes.find(pid); 
        if (it == processes.end()) { 
            return nullptr; 
        } 
        return it->second; 
    }
    */

    void markRunning(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);
        runningProcesses[process->getId()] = process;
    }

    void markFinished(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        int pid = process->getId();

        ProcessCache info;
        info.pid = pid;
        info.name = process->getName();
        info.coreId = process->getCpuCore();
        info.executedInstructions = process->getExecutedInstructions();
        info.totalInstructions = process->getTotalInstructions();
        info.logs = process->getLogs();

        runningProcesses.erase(pid);

        finishedProcessCache[pid] = info;
    }

    void printProcessReport() {
        std::lock_guard<std::mutex> lock(mutex);

        printCpuStatus();

        std::cout << "-----------------------------\n";
        std::cout << "Running:\n";
        for (const auto& pair : runningProcesses) {
            printProcessInfo(pair.second);
        }
        
        std::cout << "\nFinished:\n";

        for (const auto& pair : finishedProcessCache) {
            printFinishedProcessInfo(pair.second);
        }
    }

private:
    std::map<int, std::shared_ptr<Process>> runningProcesses;
    std::map<int, ProcessCache> finishedProcessCache;
    //std::map<int, std::shared_ptr<Process>> processes;

    std::mutex mutex;

    std::vector<std::unique_ptr<CoreWorker>> workers;
    std::unique_ptr<IScheduler> scheduler;

    int numCores;
    int nextCore = 0;
    
    std::atomic<bool> generateProcesses{false};
    unsigned long long lastGenerationTick = 0;
    int generatedProcessCount = 0;

    void run() override {
        while (true) {
            // process generation
            if (generateProcesses.load()) {
                unsigned long long currentTick = Globals::get().cpuCycles.load();
                unsigned long long freq = Globals::get().batchProcessFreq;

                if (currentTick - lastGenerationTick >= freq) {
                    generatedProcessCount++;
                    
                    
                    std::string name = std::string("p") + (generatedProcessCount < 10 ? "0" : "") + std::to_string(generatedProcessCount);
                    
                    int minIns = Globals::get().minIns;
                    int maxIns = Globals::get().maxIns;
                    int insCount = minIns + (std::rand() % (maxIns - minIns + 1));

                    auto newProcess = ProcessFactory::createProcess(name, generatedProcessCount, insCount);
                    addProcess(newProcess);
                    
                    lastGenerationTick = currentTick;
                }
            }

            // dispatcher
            for (int i = 0; i < numCores; i++) {
                if (!scheduler->isQueueEmpty(i)) {
                    if (workers[i]->isIdle()) {
                        auto process = scheduler->getNextProcess(i);
                        if (process != nullptr) {
                            workers[i]->assignProcess(process);
                        }
                    }
                }
            }

            std::this_thread::yield(); 
        }
    }

    void printProcessInfo(const std::shared_ptr<Process>& process) {
        int coreId = process->getCpuCore();

        std::cout << "Name: " << process->getName() << " | Core: ";

        if (coreId == -1) std::cout << "N/A";
        else if (coreId == -2) std::cout << "Finished";
        else std::cout << coreId;

        std::cout << " | " << process->getExecutedInstructions()
                  << " / " << process->getTotalInstructions() << "\n";
    }

    void printCpuStatus() {
        int coresUsed = 0;

        for (const auto& worker : workers) {
            if (!worker->isIdle()) {
                coresUsed++;
            }
        }
        
        int coresAvailable = numCores - coresUsed;
        double cpuUtilization = 0.0;

        if (numCores > 0) {
            cpuUtilization = (static_cast<double>(coresUsed) / numCores) * 100.0;
        }

        std::cout << "CPU Status:\n";
        std::cout << "Cores Available: " << coresAvailable << "\n";
        std::cout << "Cores Used: " << coresUsed << "\n";
        std::cout << "CPU Utilization: " << cpuUtilization << "%\n";
    }
};