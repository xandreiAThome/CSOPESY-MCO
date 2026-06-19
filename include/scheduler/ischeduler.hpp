#pragma once

#include <memory>
#include "scheduler/process.hpp"

class IScheduler {
public:
    virtual ~IScheduler() = default;

    virtual void enqueue(std::shared_ptr<Process> process, int coreId) = 0;
    virtual std::shared_ptr<Process> getNextProcess(int coreId) = 0;
    virtual bool isQueueEmpty(int coreId) = 0;
    virtual int getQueueSize(int coreId) = 0;
};