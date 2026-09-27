#ifndef OPERATOR_CONSOLE_H
#define OPERATOR_CONSOLE_H

#include "Commands.h"

#include <memory>
#include <string>
#include <vector>

//Invoker
//Owns every command it has executed successfully (for cancellation).
//Rejected commands are destroyed immediately.
class OperatorConsole {
public:
    explicit OperatorConsole(const std::string& operatorName) : operator_(operatorName) {}

    bool submit(std::unique_ptr<OperatorCommand> command);
    bool cancelLast();
    std::size_t historySize() const { return history_.size(); }

private:
    std::string operator_;
    std::vector<std::unique_ptr<OperatorCommand>> history_;
};

#endif
