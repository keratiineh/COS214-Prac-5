#include "OperatorConsole.h"

#include <iostream>
#include <utility>

bool OperatorConsole::submit(std::unique_ptr<OperatorCommand> command) {
    if (!command) return false;
    std::cout << "\n[Console:" << operator_ << "] > " << command->describe() << "\n";
    if (!command->execute()) {
        std::cout << "[Console:" << operator_ << "] command failed and was discarded\n";
        return false;
    }
    history_.push_back(std::move(command));
    return true;
}

bool OperatorConsole::cancelLast() {
    if (history_.empty()) {
        std::cout << "\n[Console:" << operator_ << "] > Cancel: nothing to cancel\n";
        return false;
    }
    std::unique_ptr<OperatorCommand> last = std::move(history_.back());
    history_.pop_back();
    std::cout << "\n[Console:" << operator_ << "] > Cancel: " << last->describe() << "\n";
    return last->undo();
}
