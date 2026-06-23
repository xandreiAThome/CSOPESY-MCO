#include "commands/add_command.hpp"
#include "process.hpp"
#include <limits>

AddCommand::AddCommand(
    const std::string& destinationVariable,
    const Operand& leftOperand,
    const Operand& rightOperand 
    ) : 
    destinationVariable(destinationVariable),
    leftOperand(leftOperand),
    rightOperand(rightOperand) {
}

void AddCommand::execute(Process& process) {
    unsigned int result = leftOperand.resolve(process) + rightOperand.resolve(process);

    if (result > std::numeric_limits<uint16_t>::max()) {
        result = std::numeric_limits<uint16_t>::max();
    }

    process.setVariable(destinationVariable, static_cast<uint16_t>(result));
}