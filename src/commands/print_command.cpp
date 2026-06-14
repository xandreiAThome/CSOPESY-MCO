#include "commands/print_command.hpp"
#include <iostream>
#include <string>

PrintCommand::PrintCommand(int pid): pid(pid) {
	toPrint = "Hello world from process" + pid;
}

void PrintCommand::execute() {
	std::cout << "Hello World";
}