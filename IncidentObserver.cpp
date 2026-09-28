#include "IncidentObserver.h"
#include "Incident.h"

#include <iostream>

//AuditLogObserver

void AuditLogObserver::onStatusChanged(const Incident& incident, IncidentStatus oldStatus, IncidentStatus newStatus) {
    std::string entry = "[AUDIT] Incident #" + std::to_string(incident.id()) +
                        " (" + incident.location() + "): " +
                        toString(oldStatus) + " -> " + toString(newStatus) +
                        " [Severity " + std::to_string(incident.severity()) + "]";
    history_.push_back(entry);
    std::cout << "  " << entry << "\n";
}

void AuditLogObserver::printSummary() const {
    std::cout << "\n=== Audit Log Trail (" << history_.size() << " entries) ===\n";
    for (const auto& entry : history_) {
        std::cout << "    " << entry << "\n";
    }
    std::cout << "=========================================\n";
}

//DashboardObserver

DashboardObserver::DashboardObserver(const std::string& terminalName)
    : terminalName_(terminalName) {}

void DashboardObserver::onStatusChanged(const Incident& incident, IncidentStatus /*oldStatus*/, IncidentStatus newStatus) {
    std::cout << "  [Dashboard:" << terminalName_ << "] ALERT: "
              << incident.label() << " status updated to ["
              << toString(newStatus) << "]\n";
}
