#pragma once

enum SchedulerType {
	FCFS,
	RR
};

struct Config{
	int numCpu;
	SchedulerType scheduler;
	int quantumCycles;
	int batchProcessFreq;
	int minIns;
	int maxIns;
	int delayPerExec;

	static Config& get();
	bool init();

};