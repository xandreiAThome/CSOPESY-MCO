#pragma once

#include "commands/icommand.hpp"
#include <string>
#include <cstdint>            

class DeclareCommand : public ICommand {
public:
    DeclareCommand(const std::string& varName, uint16_t value);
    void execute(Process& process) override;

private:
    std::string varName;
    uint16_t value;
};