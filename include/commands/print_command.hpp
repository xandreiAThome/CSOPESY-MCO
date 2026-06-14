#pragma once
#include "commands/icommand.hpp"
#include <string>

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid);
	void execute() override;

private:
	int pid;
	std::string toPrint;
};

