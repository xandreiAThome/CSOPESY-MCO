#include "commands/subtract_command.hpp"
#include "scheduler/process.hpp"

SubtractCommand::SubtractCommand(
    const std::string& destinationVariable,
    const Operand& leftOperand,
    const Operand& rightOperand
) : destinationVariable(destinationVariable), 
    leftOperand(leftOperand), 
    rightOperand(rightOperand) {
}

void SubtractCommand::execute(Process& process) {
    int result = leftOperand.resolve(process) - rightOperand.resolve(process);

    if (result < 0) {
        result = 0;
    }

    process.setVariable(destinationVariable, static_cast<uint16_t>(result));
}