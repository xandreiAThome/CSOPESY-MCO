#include "scheduler/fcfsscheduler.hpp"

FCFSScheduler::FCFSScheduler(int cores)
    : processQueues(cores),
    queueMutexes(cores) {
}

void FCFSScheduler::enqueue(std::shared_ptr<Process> process, int coreId) {
    std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
    processQueues[coreId].push(process);
}

std::shared_ptr<Process> FCFSScheduler::getNextProcess(int coreId) {
    std::lock_guard<std::mutex> lock(queueMutexes[coreId]);

    if (processQueues[coreId].empty()) {
        return nullptr;
    }

    auto nextProcess = processQueues[coreId].front();
    processQueues[coreId].pop();

    return nextProcess;
}

bool FCFSScheduler::isQueueEmpty(int coreId) {
    if (coreId < 0 || coreId >= static_cast<int>(processQueues.size())) {
        return true;
    }

    std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
    return processQueues[coreId].empty();
}

int FCFSScheduler::getQueueSize(int coreId) {
    if (coreId < 0 || coreId >= static_cast<int>(processQueues.size())) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(queueMutexes[coreId]);
    return static_cast<int>(processQueues[coreId].size());
}
