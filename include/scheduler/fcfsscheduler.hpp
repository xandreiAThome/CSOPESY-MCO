#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <memory>

#include "scheduler/process.hpp"
#include "scheduler/ischeduler.hpp"

class FCFSScheduler : public IScheduler {
public:
     FCFSScheduler(int cores)
        : processQueues(cores),
        queueMutexes(cores)
    {
      //   std::cout << "FCFSScheduler created with " << cores << " cores\n";
    }

    void enqueue(std::shared_ptr<Process> process, int coreId)  {
        std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
        processQueues[coreId].push(process);
    }

    std::shared_ptr<Process> getNextProcess(int coreId) override  {
        std::lock_guard<std::mutex> lock(queueMutexes[coreId]);

        if (processQueues[coreId].empty()) {
            return nullptr;
        }

        auto nextProcess = processQueues[coreId].front();
        processQueues[coreId].pop();

        return nextProcess;
    }

    bool isQueueEmpty(int coreId) override {
        if (coreId < 0 || coreId >= static_cast<int>(processQueues.size())) {
            return true;
        }

        std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
        return processQueues[coreId].empty();
    }

    int getQueueSize(int coreId) override {
        if (coreId < 0 || coreId >= static_cast<int>(processQueues.size())) {
            return 0;
        }

        std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
        return static_cast<int>(processQueues[coreId].size());
    }

    // sortProcesses()

private:
    std::vector<std::queue<std::shared_ptr<Process>>> processQueues;
    std::vector<std::mutex> queueMutexes;
};