#include "CommsCentre.h"

#include <iostream>

std::string toString(AlertLevel level) {
    switch (level) {
        case AlertLevel::Info:     return "INFO";
        case AlertLevel::Warning:  return "WARNING";
        case AlertLevel::Evacuate: return "EVACUATE";
    }
    return "UNKNOWN";
}

bool CommsCentre::broadcast(AlertLevel level, const std::string& zone, const std::string& message) {
    if (message.empty()) {
        std::cout << "  [Comms] REJECTED: empty alert for " << zone << "\n";
        return false;
    }
    std::cout << "  [Comms] " << toString(level) << " to " << zone << ": " << message << "\n";
    return true;
}

bool CommsCentre::retract(const std::string& zone) {
    std::cout << "  [Comms] RETRACTION to " << zone << ": disregard the previous alert\n";
    return true;
}
