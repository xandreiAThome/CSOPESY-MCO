#pragma once

class Process;

enum CommandType {
	PRINT,
	DECLARE,
	ADD,
	SUBTRACT,
	SLEEP,
	FOR
};

class ICommand {
public:
	virtual ~ICommand() = default;
	virtual void execute(Process& process) = 0;
};