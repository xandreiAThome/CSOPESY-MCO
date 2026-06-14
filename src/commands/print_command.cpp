#include "commands/print_command.hpp"
#include "scheduler/fcfsscheduler.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <filesystem>

PrintCommand::PrintCommand(int pid): pid(pid) {
	if (pid < 10) {
		toPrint = "\"Hello world from screen_0" + std::to_string(pid) + "\"";
	}
	else {
		toPrint = "\"Hello world from screen_" + std::to_string(pid) + "\"";
	}
}

void PrintCommand::execute() {
	int cpuCore = FCFSScheduler::get().getProcess(pid)->getCpuCore();
	std::string filename = std::to_string(pid) + "_out.txt";

	bool fileExist = std::filesystem::exists(filename);

	std::ofstream writeFile(filename, std::ios::app);

	if (!writeFile) {
		return;
	}

	// Write header if file is newly created
	if (!fileExist) {
		writeFile << FCFSScheduler::get().getProcess(pid)->getName() << '\n'
			<< "Logs: \n\n";
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	writeFile << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p) ") << "Core: " << cpuCore << ' '
		<< toPrint << '\n';

	writeFile.close();
}