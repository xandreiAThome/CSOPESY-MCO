#pragma once
#include <vector>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <chrono>
#include <queue>
#include <atomic>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "scheduler/process.hpp"
#include "globals.hpp"

//a template for an FCFS scheduler
class FCFSScheduler {

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // make singleton for now
    static FCFSScheduler& get() {
        static FCFSScheduler instance(Globals::get().numCpu);
        return instance;
    }

    // Add a process to the scheduler 
    // Currently rotates core assignment
    void addProcess(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(queueMutex); // only 1 thread allowed

        processQueues[nextCore].push(process);
        process->setCpuCore(nextCore);
        std::cout << "Added Process " << process->getId()
            << " to Core " << nextCore + 1 << " queue.\n";
        


        nextCore = (nextCore + 1) % numCores;
    }

    // Generate processes indefinitely with 1 thread
    /*
    void schedulerStart() {
        if (generatingProcesses.load()) {
            std::cout << "Scheduler is already generating processes.\n";
            return;
        }

        generatingProcesses.store(true);
        std::thread([this]() {

            while (generatingProcesses) {
                waitForTicks(Globals::get().batchProcessFreq);

                int nInstructions = rand() % (Globals::get().maxIns - Globals::get().minIns + 1) + Globals::get().minIns;

                std::shared_ptr<Process> process = std::make_shared<Process>(
                    "process_" + std::to_string(nextPid),
                    nextPid,
                    nInstructions
                );

                for (int i = 0; i < nInstructions; i++) {
                    process->addCommand(CommandType::PRINT);
                }

                addProcess(process);

                nextPid++;
            }
            }).detach();

    }
    */

    // START GENERATING NEW PROCESSES VIA SEPARATE THREAD
    // Limit to 10 Processes
    void schedulerStart() {
        if (generatingProcesses.load()) {
            std::cout << "Scheduler is already generating processes.\n";
            return;
        }

        generatingProcesses.store(true);
        std::thread([this]() {

            for (int i = 0; i < 10 && generatingProcesses.load(); i++) {
                waitForTicks(Globals::get().batchProcessFreq);

                int nInstructions = rand() % (Globals::get().maxIns - Globals::get().minIns + 1) + Globals::get().minIns;

                std::shared_ptr<Process> process = std::make_shared<Process>(
                    "process_" + std::to_string(nextPid),
                    nextPid,
                    nInstructions
                );

                this->processes[nextPid] = process;

                for (int j = 0; j < nInstructions; j++) {
                    process->addCommand(CommandType::PRINT);
                }

                addProcess(process);

                nextPid++;
            }
            }).detach();

    }

    void waitForTicks(unsigned long long ticksToWait) {
        unsigned long long startTick = Globals::get().cpuCycles.load();

        while (Globals::get().cpuCycles.load() - startTick < ticksToWait) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
     
    // STOP GENERATING PROCESSES
    void schedulerStop() {
        std::cout << "Stopping process generation\n";
        generatingProcesses.store(false);
    }

   

    // START CPU WORKER THREADS
    void runScheduler() {
        if (schedulerRunning.load()) {
            std::cout << "Scheduler is already running.\n";
            return;
        }

        schedulerRunning.store(true);

        for (int core = 0; core < numCores; core++) {
            std::thread(&FCFSScheduler::cpuWorkerLoop, this, core).detach();
        }

        std::cout << "Scheduler started with " << numCores << " cores.\n";
    }

    // Get specific process (maybe move to different class?)
    std::shared_ptr<Process> getProcess(int pid) {
        std::lock_guard<std::mutex> lock(processesMutex);

        auto it = processes.find(pid);

        if (it == processes.end()) {
            return nullptr;
        }

        return it->second;
    }

private:
    int numCores;
    int nextCore = 0;
    int nextPid = 1;



    std::vector<std::queue<std::shared_ptr<Process>>> processQueues; // One ready queue for each core

    // TODO: Change to lightweight info cache instead of storing process pointers
    std::unordered_map<int, std::shared_ptr<Process>> processes; // All generated processes
    std::vector<std::shared_ptr<Process>> finishedProcesses; // All finished processes 

    std::atomic<bool> generatingProcesses = false;
    std::atomic<bool> schedulerRunning = false;

    // Locks
    std::mutex processesMutex;
    std::mutex queueMutex;
    std::mutex finishedMutex;
    

    // 1 thread per cpu worker
    void cpuWorkerLoop(int coreId) {
        while (schedulerRunning.load()) {
            std::shared_ptr<Process> currentProcess = nullptr;
            bool hasProcess = false;

            {
                std::lock_guard<std::mutex> lock(queueMutex);

                if (!processQueues[coreId].empty()) {
                    currentProcess = processQueues[coreId].front();
                    processQueues[coreId].pop();
                    hasProcess = true;
                }
            }

            if (hasProcess) {
                std::cout << "Core " << coreId + 1
                    << " running Process "
                    << currentProcess->getId() << "\n";

                currentProcess->setState(Process::RUNNING);
                int delay = Globals::get().delayPerExec;

                while (!currentProcess->hasFinished()) {
                    currentProcess->executeInstruction();

                    if (delay > 0) waitForTicks(delay);
                }

                currentProcess->setState(Process::FINISHED);
                currentProcess->setCpuCore(-1);

                // TODO: Change to info cache and clear process pointer from memory
                std::lock_guard<std::mutex> lock(finishedMutex);
                finishedProcesses.push_back(currentProcess);

                std::cout << "Core " << coreId + 1
                    << " completed Process "
                    << currentProcess->getId() << "\n";
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }

};