#include "config.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

Config& Config::get() {
	static Config instance;
	return instance;
}

// return true if success, false if otherwise

// std::cout was used for debugging. 
// only displays if you remove clear screen from initialize command in console_state.cpp
bool Config::init() {
	std::string setting;
	std::vector<std::string> settingVec;

	std::ifstream configFile("src/config.txt");

	if (!configFile) {
		std::cout << "Failed to open config.txt\n";
		return false;
	}

	while (std::getline(configFile, setting)) {
        std::string settingVal = setting.substr(setting.find(' ') + 1);
        settingVec.push_back(settingVal);
	}

	if (settingVec.size() < 7) {
		std::cout << "config.txt is missing settings\n";
		return false;
	}

	numCpu = std::stoi(settingVec[0]);
	
	if (settingVec[1] == "\"fcfs\"") scheduler = SchedulerType::FCFS;
	else if (settingVec[1] == "\"rr\"") scheduler = SchedulerType::RR;
	else std::cout << "Invalid scheduler type in config.txt \n";

	quantumCycles = std::stoi(settingVec[2]);
	batchProcessFreq = std::stoi(settingVec[3]);
	minIns = std::stoi(settingVec[4]);
	maxIns = std::stoi(settingVec[5]);
	delayPerExec = std::stoi(settingVec[6]);

	configFile.close();

	return true;
}