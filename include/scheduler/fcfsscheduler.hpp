#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <memory>

#include "scheduler/process.hpp"
#include "scheduler/ischeduler.hpp"

class FCFSScheduler : public IScheduler {
public:
     FCFSScheduler(int cores);

    void enqueue(std::shared_ptr<Process> process, int coreId) override;
    std::shared_ptr<Process> getNextProcess(int coreId) override;
    bool isQueueEmpty(int coreId) override;
    int getQueueSize(int coreId) override;

private:
    std::vector<std::queue<std::shared_ptr<Process>>> processQueues;
    std::vector<std::mutex> queueMutexes;
};