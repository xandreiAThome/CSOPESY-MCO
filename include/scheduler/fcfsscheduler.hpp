#pragma once
#include <vector>
#include "scheduler/process.hpp"

//a template for an FCFS scheduler
class FCFSScheduler {
private:
    int numCores;
    std::vector<std::vector<Process>> processQueues; // One queue for each core

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const Process& process, int core = 0) {
        if (core >= 0 && core < numCores) {
            processQueues[core].push_back(process);
        }
        else {
            std::cerr << "Invalid core specified for process addition.\n";
        }
    }

    // Sort the process queues based on remaining instructions (FCFS)
    void sortProcessQueues() {
        for (auto& queue : processQueues) {
            std::sort(queue.begin(), queue.end(), [](const Process& a, const Process& b) {
                return a.getRemainingInstructions() > b.getRemainingInstructions();
                });
        }
    }

    // Run the scheduler
    void runScheduler() {
        while (!processQueues[0].empty()) {
            for (int core = 0; core < numCores; ++core) {
                if (!processQueues[core].empty()) {
                    Process currentProcess = processQueues[core].back();
                    processQueues[core].pop_back();

                    while (!currentProcess.hasFinished()) {
                        currentProcess.executeInstruction();
                    }

                    std::cout << "Process " << currentProcess.getRemainingInstructions() << " completed on Core " << core + 1 << ".\n";
                }
            }
        }
    }
};