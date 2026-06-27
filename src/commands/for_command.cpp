#include "commands/for_command.hpp"
#include "scheduler/process.hpp"

#include <stdexcept>
#include <utility>

ForCommand::ForCommand(
    std::vector<std::shared_ptr<ICommand>> commands,
    int iters
) :
    commands(std::move(commands)),
    iters(iters) {
}

void ForCommand::execute(Process& process) {
    throw std::runtime_error("ForCommand should be expanded before execution.");
}

void ForCommand::expandInto(
    std::vector<std::shared_ptr<ICommand>>& output,
    int depth
) const {
    if (depth > 3) {
        throw std::runtime_error("FOR loop nesting cannot exceed 3 levels.");
    }

    if (iters < 0) {
        throw std::runtime_error("FOR loop iterations cannot be negative.");
    }

    for (int i = 0; i < iters; i++) {
        for (const auto& command : commands) {
            auto nestedFor = std::dynamic_pointer_cast<ForCommand>(command);

            if (nestedFor) {
                nestedFor->expandInto(output, depth + 1);
            }
            else {
                output.push_back(command);
            }
        }
    }
}