#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <mutex>

#include "scheduler/process.hpp"
#include "scheduler/ietthread.hpp"
#include "scheduler/fcfsscheduler.hpp"
#include "scheduler/core_worker.hpp"
#include "globals.hpp"

class GlobalScheduler {
public:
    GlobalScheduler(int cores)
        : numCores(cores),
        scheduler(std::make_unique<FCFSScheduler>(cores))
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
    }

    void addProcess(std::shared_ptr<Process> process) {
        int assignedCore;

        {
            std::lock_guard<std::mutex> lock(mutex);
            processes[process->getId()] = process;

            assignedCore = nextCore;          
            nextCore = (nextCore + 1) % numCores;
        }

        process->setArrivalTime(Globals::get().cpuCycles);
        

        scheduler->enqueue(process, assignedCore);
        markRunning(process);

        // std::cout << "Added Process " << process->getId()       << " to Core " << assignedCore + 1 << " queue.\n";
    }

    std::shared_ptr<Process> getProcess(int pid) { 
        std::lock_guard<std::mutex> lock(mutex); 
        auto it = processes.find(pid); 
        if (it == processes.end()) { 
            return nullptr; 
        } 
        
        return it->second; 
    }

    std::shared_ptr<Process> getNextProcess(int coreId) {
        auto process = scheduler->getNextProcess(coreId);

        if (process != nullptr) {
            markRunning(process);
        }

        return process;
    }

    void markRunning(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        runningProcesses[process->getId()] = process;
    }

    void markFinished(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        runningProcesses.erase(process->getId());
        finishedProcesses.push_back(process);
    }

    void printProcessReport() {
        std::lock_guard<std::mutex> lock(mutex);

        std::cout << "CPU Utilization: " << "???\n";
        std::cout << "Cores Used: " << "???\n";
        std::cout << "Cores Available: " << "???\n";

        std::cout << "-----------------------------\n";
        std::cout << "Running:\n";

        if (!runningProcesses.empty()) {
            for (const auto& pair : runningProcesses) {
                printProcessInfo(pair.second);
            }
        }
 

        std::cout << "\nFinished:\n";

        if (!finishedProcesses.empty()) {
            for (const auto& process : finishedProcesses) {
                printProcessInfo(process);
            }
        }

    }


private:
    std::unordered_map<int, std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;
    std::unordered_map<int, std::shared_ptr<Process>> processes; // All generated processes

    std::mutex mutex;

    std::vector<std::unique_ptr<CoreWorker>> workers;
    std::unique_ptr<IScheduler> scheduler;

    int numCores;
    int nextCore = 0;

    void printProcessInfo(const std::shared_ptr<Process>& process) {
        int coreId = process->getCpuCore();

        std::cout << "Name: " << process->getName()
            << " | Core: ";

        if (coreId == -1) {
            std::cout << "N/A";
        }
        else if (coreId == -2) {
            std::cout << "Finished";
        }
        else {
            std::cout << coreId + 1;
        }

        std::cout << " | "
            << process->getExecutedInstructions()
            << " / "
            << process->getTotalInstructions()
            << "\n";
    }
};