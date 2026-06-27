#include "commands/add_command.hpp"
#include "scheduler/process.hpp"
#include <limits>
#include <cstdint>

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
    std::string leftLog = leftOperand.toLogString(process);
    std::string rightLog = rightOperand.toLogString(process);

    unsigned int result = leftOperand.resolve(process) + rightOperand.resolve(process);

    if (result > std::numeric_limits<uint16_t>::max()) {
        result = std::numeric_limits<uint16_t>::max();
    }

    process.setVariable(destinationVariable, static_cast<uint16_t>(result));

    process.addLog(
        destinationVariable + " = " +
        leftLog + " + " +
        rightLog + " is performed. " +
        destinationVariable + " is now " + std::to_string(result)
    );
}