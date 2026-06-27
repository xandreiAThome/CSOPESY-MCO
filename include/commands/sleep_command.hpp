#pragma once

#include "commands/icommand.hpp"
#include <cstdint>

class SleepCommand : public ICommand {
public:
    SleepCommand(uint8_t ticks);
    void execute(Process& process) override;

private:
    uint8_t ticks;
};