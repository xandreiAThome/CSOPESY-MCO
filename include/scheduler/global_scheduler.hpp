#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <map>

#include "scheduler/process.hpp"
#include "scheduler/ietthread.hpp"
#include "scheduler/fcfsscheduler.hpp"
#include "scheduler/core_worker.hpp"
#include "scheduler/process_cache.hpp"
#include "globals.hpp"

class GlobalScheduler {
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

        // std::cout << "Added Process " << process->getId()       << " to Core " << assignedCore + 1 << " queue.\n";
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
            std::cout << coreId;
        }

        std::cout << " | "
            << process->getExecutedInstructions()
            << " / "
            << process->getTotalInstructions()
            << "\n";
    }

    void printCpuStatus() {
        int coresUsed = 0;
        int coresAvailable = 0;

        for (int coreId = 0; coreId < numCores; coreId++) {
            bool queueEmpty = scheduler->isQueueEmpty(coreId);

            bool hasRunningProcess = false;

            for (const auto& pair : runningProcesses) {
                auto process = pair.second;

                if (process->getCpuCore() == coreId) {
                    hasRunningProcess = true;
                    break;
                }
            }

            if (queueEmpty && !hasRunningProcess) {
                coresAvailable++;
            }
            else {
                coresUsed++;
            }
        }

        double cpuUtilization = 0.0;

        if (numCores > 0) {
            cpuUtilization = (static_cast<double>(coresUsed) / numCores) * 100.0;
        }

        std::cout << "CPU Status:\n";
        std::cout << "Cores Available: " << coresAvailable << "\n";
        std::cout << "Cores Used: " << coresUsed << "\n";
        std::cout << "CPU Utilization: " << cpuUtilization << "%\n";
    }

    void printFinishedProcessInfo(const ProcessCache& process) {
        std::cout << "Name: " << process.name
            << " | Core: Finished"
            << " | "
            << process.executedInstructions
            << " / "
            << process.totalInstructions
            << "\n";
    }

};