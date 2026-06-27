#include "commands/operand.hpp"
#include "scheduler/process.hpp"

Operand::Operand(bool isVariable, const std::string& variableName, uint16_t literalValue)
    : isVariable(isVariable),
    variableName(variableName),
    literalValue(literalValue) {
}

Operand Operand::variable(const std::string& variableName) {
    return Operand(true, variableName, 0);
}

Operand Operand::value(uint16_t literalValue) {
    return Operand(false, "", literalValue);
}

uint16_t Operand::resolve(Process& process) const {
    if (isVariable) {
        return process.getVariable(variableName);
    }

    return literalValue;
}

std::string Operand::toLogString(Process& process) const {
    if (isVariable) {
        uint16_t value = process.getVariable(variableName);
        return variableName + " (" + std::to_string(value) + ")";
    }

    return std::to_string(literalValue);
}