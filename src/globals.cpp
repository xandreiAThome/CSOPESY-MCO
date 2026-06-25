#include "globals.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

Globals& Globals::get() {
	static Globals instance;
	return instance;
}

// return true if success, false if otherwise

// std::cout was used for debugging. 
// only displays if you remove clear screen from initialize command in console_state.cpp
bool Globals::init() {
	std::string setting;
	std::vector<std::string> settingVec;

	std::ifstream configFile("config.txt");

	if (!configFile) {
		std::cout << "Failed to open config.txt\n";
		return false;
	}

	while (std::getline(configFile, setting)) {
		size_t spacePos = setting.find(' ');

		if (spacePos == std::string::npos) {
			continue;
		}

		std::string settingVal = setting.substr(spacePos + 1);

		// trim trailing whitespace
		size_t end = settingVal.find_last_not_of(" \t\r");
		if (end != std::string::npos) {
			settingVal = settingVal.substr(0, end + 1);
		}

		if (!settingVal.empty()) {
			settingVec.push_back(settingVal);
		}
	}

	if (settingVec.size() < 7) {
		std::cout << "config.txt is missing settings\n";
		return false;
	}

	// Set scheduler
	if (settingVec[1] == "\"fcfs\"") scheduler = SchedulerType::FCFS;
	else if (settingVec[1] == "\"rr\"") scheduler = SchedulerType::RR;
	else return false;

	// Set numeric settings
	try {
		numCpu = std::stoi(settingVec[0]);
		quantumCycles = std::stoi(settingVec[2]);
		batchProcessFreq = std::stoi(settingVec[3]);
		minIns = std::stoi(settingVec[4]);
		maxIns = std::stoi(settingVec[5]);
		delayPerExec = std::stoi(settingVec[6]);
	}
	catch (const std::exception&) {
		std::cout << "Invalid numeric setting in config.txt\n";
		return false;
	}
	
	configFile.close();

	return true;
}