#include "commands/sleep_command.hpp"
#include "scheduler/process.hpp"

SleepCommand::SleepCommand(uint8_t ticks)
    : ticks(ticks) {
}

void SleepCommand::execute(Process& process) {
    process.sleep(ticks);
}