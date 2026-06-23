#pragma once

#include "commands/icommand.hpp"
#include "commands/operand.hpp"
#include <string>

class AddCommand : public ICommand {
public:
    AddCommand(
        const std::string& destinationVariable,
        const Operand& leftOperand,
        const Operand& rightOperand
    );

    void execute(Process& process) override;

private:
    std::string destinationVariable;
    Operand leftOperand;
    Operand rightOperand;
};