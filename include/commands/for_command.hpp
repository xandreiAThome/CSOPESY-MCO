#pragma once

#include "commands/icommand.hpp"

#include <cstdint>
#include <vector>
#include <memory>

class ForCommand : public ICommand {
public:
    ForCommand(std::vector<std::shared_ptr<ICommand>> commands, int iters);

    void execute(Process& process) override;
    void expandInto(std::vector<std::shared_ptr<ICommand>>& output, int depth = 1) const;

private:
    std::vector<std::shared_ptr<ICommand>> commands;
    int iters;
};