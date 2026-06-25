#include "globals.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

Globals& Globals::get() {
	static Globals instance;
	return instance;
}

// return true if success, false if otherwise

// std::cout was used for debugging. 
// only displays if you remove clear screen from initialize command in console_state.cpp
bool Globals::init() {
	std::string setting;
	std::unordered_map<std::string, std::string> configs;

	std::ifstream configFile("config.txt");

	if (!configFile) {
		std::cout << "Failed to open config.txt\n";
		return false;
	}

	while (std::getline(configFile, setting)) {
		// Strip comments starting with '#'
		size_t commentPos = setting.find('#');
		if (commentPos != std::string::npos) {
			setting = setting.substr(0, commentPos);
		}

		// Find key-value space separator
		size_t spacePos = setting.find(' ');
		if (spacePos == std::string::npos) {
			continue;
		}

		std::string key = setting.substr(0, spacePos);
		std::string settingVal = setting.substr(spacePos + 1);

		// Trim key
		size_t keyEnd = key.find_last_not_of(" \t\r\n");
		if (keyEnd != std::string::npos) {
			key = key.substr(0, keyEnd + 1);
		}

		// Trim settingVal
		size_t start = settingVal.find_first_not_of(" \t");
		if (start != std::string::npos) {
			settingVal = settingVal.substr(start);
		} else {
			settingVal.clear();
		}
		size_t end = settingVal.find_last_not_of(" \t\r\n");
		if (end != std::string::npos) {
			settingVal = settingVal.substr(0, end + 1);
		}

		if (!key.empty() && !settingVal.empty()) {
			configs[key] = settingVal;
		}
	}

	configFile.close();

	// Check for all required config keys
	std::vector<std::string> requiredKeys = {
		"num-cpu", "scheduler", "quantum-cycles", "batch-process-freq",
		"min-ins", "max-ins", "delay-per-exec"
	};

	for (const auto& rKey : requiredKeys) {
		if (configs.find(rKey) == configs.end()) {
			std::cout << "config.txt is missing settings: " << rKey << "\n";
			return false;
		}
	}

	// Set scheduler
	if (configs["scheduler"] == "\"fcfs\"") scheduler = SchedulerType::FCFS;
	else if (configs["scheduler"] == "\"rr\"") scheduler = SchedulerType::RR;
	else return false;

	// Set numeric settings
	try {
		numCpu = std::stoi(configs["num-cpu"]);
		quantumCycles = std::stoi(configs["quantum-cycles"]);
		batchProcessFreq = std::stoi(configs["batch-process-freq"]);
		minIns = std::stoi(configs["min-ins"]);
		maxIns = std::stoi(configs["max-ins"]);
		delayPerExec = std::stoi(configs["delay-per-exec"]);
	}
	catch (const std::exception&) {
		std::cout << "Invalid numeric setting in config.txt\n";
		return false;
	}

	// Validation
	if (numCpu <= 0 || quantumCycles <= 0 || batchProcessFreq <= 0 ||
		minIns < 0 || maxIns < 0 || minIns > maxIns || delayPerExec < 0) {
		std::cout << "Invalid settings values in config.txt\n";
		return false;
	}

	return true;
}