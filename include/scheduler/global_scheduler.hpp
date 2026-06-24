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
#include <fstream>
#include <sstream>
#include <filesystem>

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

        process->setArrivalTime(Globals::get().cpuCycles);

        {
            std::lock_guard<std::mutex> lock(mutex);

            const std::string name = process->getName();

            if (processExistsNoLock(name)) {
                return;
            }

            readyProcesses[name] = process;

            assignedCore = nextCore;
            nextCore = (nextCore + 1) % numCores;

            
        }

        scheduler->enqueue(process, assignedCore);
        
    }

    std::shared_ptr<Process> getNextProcess(int coreId) {
      
        auto process = scheduler->getNextProcess(coreId);

        if (process != nullptr) {
            markRunning(process);
        }

        return process;
    }

    
    bool getProcessCache(const std::string& name, ProcessCache& outCache) {
        std::lock_guard<std::mutex> lock(mutex);

        auto readyIt = readyProcesses.find(name);
        if (readyIt != readyProcesses.end()) {
            outCache = createCacheFromProcess(readyIt->second);
            return true;
        }

        auto waitingIt = waitingProcesses.find(name);
        if (waitingIt != waitingProcesses.end()) {
            outCache = createCacheFromProcess(waitingIt->second);
            return true;
        }

        auto runningIt = runningProcesses.find(name);
        if (runningIt != runningProcesses.end()) {
            outCache = createCacheFromProcess(runningIt->second);
            return true;
        }

        auto finishedIt = finishedProcessCache.find(name);
        if (finishedIt != finishedProcessCache.end()) {
            outCache = finishedIt->second;
            return true;
        }

        return false;
    }
    void markWaiting(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        std::string name = process->getName();

        readyProcesses.erase(name);
        runningProcesses.erase(name);

        waitingProcesses[name] = process;
        process->setState(Process::ProcessState::WAITING);
    }

    void markRunning(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        std::string name = process->getName();

        readyProcesses.erase(name);
        waitingProcesses.erase(name);

        runningProcesses[name] = process;
        process->setState(Process::ProcessState::RUNNING);
    }

    void markFinished(std::shared_ptr<Process> process) {
        std::lock_guard<std::mutex> lock(mutex);

        std::string name = process->getName();

        ProcessCache info = createCacheFromProcess(process);

        readyProcesses.erase(name);
        waitingProcesses.erase(name);
        runningProcesses.erase(name);

        finishedProcessCache[name] = info;
        process->setState(Process::ProcessState::FINISHED);
    }

    std::string buildProcessReport() {
        std::lock_guard<std::mutex> lock(mutex);

        std::ostringstream out;

        writeCpuStatus(out);

        out << "-----------------------------\n";
        out << "Running:\n";

        for (const auto& pair : runningProcesses) {
            writeProcessInfo(out, pair.second);
        }

        out << "\nFinished:\n";

        for (const auto& pair : finishedProcessCache) {
            writeFinishedProcessInfo(out, pair.second);
        }

        return out.str();
    }

    void printProcessReport() {
        std::cout << buildProcessReport();
    }

    void writeProcessReportToFile() {
        std::string filename = "csopesy-log.txt";

        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cout << "Failed to open report file: " << filename << "\n";
            return;
        }

        file << buildProcessReport();
        file.close();

        std::cout << "Report generated at "
            << std::filesystem::absolute(filename)
            << "!\n";
    }

    bool processExists(const std::string& pName) {
        std::lock_guard<std::mutex> lock(mutex);
        return processExistsNoLock(pName);
    }

    // Manual creation (screen -s)
    void createProcess(const std::string& name) {
        if (processExists(name)) {
            return;
        }

        int newPid = generatePid();

        int minIns = Globals::get().minIns;
        int maxIns = Globals::get().maxIns;
        int insCount = minIns + (std::rand() % (maxIns - minIns + 1));

        auto newProcess = ProcessFactory::createProcess(
            name,
            newPid,
            insCount
        );

        addProcess(newProcess);
    }

    // Wake up sleeping / waiting processes
    void wakeReadyProcesses() {
        std::vector<std::shared_ptr<Process>> readyToWake;

        {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto it = waitingProcesses.begin(); it != waitingProcesses.end(); ) {
                auto process = it->second;

                if (process->isReadyToWake()) {
                    process->setState(Process::READY);

                    readyProcesses[process->getName()] = process;
                    readyToWake.push_back(process);

                    it = waitingProcesses.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        for (auto& process : readyToWake) {
            scheduler->enqueue(process, process->getCpuCore());
        }
    }

    // Stop all threads
    void stop() {
        generateProcesses.store(false);

        for (auto& worker : workers) {
            worker->stop();
        }

        IETThread::stop();
    }

    bool allWorkersFinished() const {
        for (const auto& worker : workers) {
            if (!worker->finished()) {
                return false;
            }
        }

        return true;
    }


private:
    std::map<std::string, std::shared_ptr<Process>> readyProcesses;
    std::map<std::string, std::shared_ptr<Process>> waitingProcesses;
    std::map<std::string, std::shared_ptr<Process>> runningProcesses;
    std::map<std::string, ProcessCache> finishedProcessCache;
    //std::map<int, ProcessCache> processes;

    std::mutex mutex;

    std::vector<std::unique_ptr<CoreWorker>> workers;
    std::unique_ptr<IScheduler> scheduler;

    int numCores;
    int nextCore = 0;
    
    std::atomic<bool> generateProcesses{false};
    
    std::atomic<int> generatedProcessCount{ 0 };

    void run() override {
        unsigned long long lastGenerationTick = 0;
        while (Globals::get().running) {
            wakeReadyProcesses(); // Repeatedly check if processes are sleeping.

            // process generation
            if (generateProcesses.load()) {
                unsigned long long currentTick = Globals::get().cpuCycles.load();
                unsigned long long freq = Globals::get().batchProcessFreq;

                if (currentTick - lastGenerationTick >= freq) {
                    int newPid = generatePid();

                    std::string name =
                        std::string("process_") +
                        (newPid < 10 ? "0" : "") +
                        std::to_string(newPid);

                    int minIns = Globals::get().minIns;
                    int maxIns = Globals::get().maxIns;
                    int insCount = minIns + (std::rand() % (maxIns - minIns + 1));

                    auto newProcess = ProcessFactory::createProcess(
                        name,
                        newPid,
                        insCount
                    );

                    addProcess(newProcess);

                    lastGenerationTick = currentTick;
                }
            }

            sleep(1);
        }
    }

    void writeProcessInfo(std::ostream& out, const std::shared_ptr<Process>& process) {
        int coreId = process->getCpuCore();

        out << "Name: " << process->getName() << " | Core: ";

        if (coreId == -1) {
            out << "N/A";
        }
        else if (coreId == -2) {
            out << "Finished";
        }
        else {
            out << coreId;
        }

        out << " | "
            << process->getExecutedInstructions()
            << " / "
            << process->getTotalInstructions()
            << "\n";
    }

    void writeCpuStatus(std::ostream& out) {
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

        out << "CPU Status:\n";
        out << "Cores Available: " << coresAvailable << "\n";
        out << "Cores Used: " << coresUsed << "\n";
        out << "CPU Utilization: " << cpuUtilization << "%\n";
    }

    void writeFinishedProcessInfo(std::ostream& out, const ProcessCache& process) {
        out << "Name: " << process.name
            << " | Core: Finished"
            << " | "
            << process.executedInstructions
            << " / "
            << process.totalInstructions
            << "\n";
    }

    ProcessCache createCacheFromProcess(const std::shared_ptr<Process>& process) {
        ProcessCache info;
        info.pid = process->getId();
        info.name = process->getName();
        info.coreId = process->getCpuCore();
        info.executedInstructions = process->getExecutedInstructions();
        info.totalInstructions = process->getTotalInstructions();
        info.logs = process->getLogs();

        return info;
    }

    bool processExistsNoLock(const std::string& pName) {
        return readyProcesses.find(pName) != readyProcesses.end()
            || waitingProcesses.find(pName) != waitingProcesses.end()
            || runningProcesses.find(pName) != runningProcesses.end()
            || finishedProcessCache.find(pName) != finishedProcessCache.end();
    }

    int generatePid() {
        return generatedProcessCount.fetch_add(1) + 1;
    }
};