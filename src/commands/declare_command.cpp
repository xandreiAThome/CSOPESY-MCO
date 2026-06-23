#include "commands/declare_command.hpp"
#include "scheduler/process.hpp"
#include <string>

DeclareCommand::DeclareCommand(const std::string& varName, uint16_t value)
    : varName(varName), value(value) {
}

void DeclareCommand::execute(Process& process) {
    process.setVariable(varName, value);
}