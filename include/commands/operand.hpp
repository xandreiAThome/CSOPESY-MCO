#pragma once

#include <string>
#include <cstdint>

class Process;

class Operand {
public:
    static Operand variable(const std::string& variableName);
    static Operand value(uint16_t literalValue);

    uint16_t resolve(Process& process) const;

private:
    Operand(bool isVariable, const std::string& variableName, uint16_t literalValue);

    bool isVariable;
    std::string variableName;
    uint16_t literalValue;
};