#pragma once

enum CommandType {
	PRINT,
	ADD
};

class ICommand {
public:
	virtual void execute() = 0;
};