#pragma once
#include <atomic>


enum SchedulerType {
	FCFS,
	RR
};

struct Globals {
	// config.txt
	int numCpu;
	SchedulerType scheduler;
	int quantumCycles;
	int batchProcessFreq;
	int minIns;
	int maxIns;
	int delayPerExec;

	bool running = true;

	// updates per frame
	std::atomic<unsigned long long> cpuCycles = 0;

	static Globals& get();
	bool init();

};