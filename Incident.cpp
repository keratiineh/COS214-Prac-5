#include "Incident.h"

#include <iostream>

std::string toString(IncidentType type) {
    switch (type) {
        case IncidentType::Fire:           return "Fire";
        case IncidentType::Medical:        return "Medical";
        case IncidentType::SecurityThreat: return "Security threat";
        case IncidentType::Infrastructure: return "Infrastructure";
    }
    return "Unknown";
}

std::string toString(IncidentStatus status) {
    switch (status) {
        case IncidentStatus::Reported:   return "Reported";
        case IncidentStatus::Dispatched: return "Dispatched";
        case IncidentStatus::Escalated:  return "Escalated";
        case IncidentStatus::Contained:  return "Contained";
        case IncidentStatus::Resolved:   return "Resolved";
    }
    return "Unknown";
}

Incident::Incident(int id, IncidentType type, const std::string& location, int severity)
    : id_(id), type_(type), location_(location), severity_(severity),
      status_(IncidentStatus::Reported) {}

#include <algorithm>
#include "IncidentObserver.h"

std::string Incident::label() const {
    return "#" + std::to_string(id_) + " " + toString(type_) + " @ " + location_;
}

bool Incident::setStatus(IncidentStatus next) {
    if (status_ == IncidentStatus::Resolved) {
        std::cout << "  [Incident " << label() << "] REJECTED: already resolved, cannot move to "
                  << toString(next) << "\n";
        return false;
    }
    if (next == status_) return true;
    IncidentStatus oldStatus = status_;
    std::cout << "  [Incident " << label() << "] " << toString(status_) << " -> "
              << toString(next) << "\n";
    status_ = next;
    notifyObservers(oldStatus, next);
    return true;
}

void Incident::attachObserver(IncidentObserver* observer) {
    if (observer && std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

void Incident::detachObserver(IncidentObserver* observer) {
    auto it = std::find(observers_.begin(), observers_.end(), observer);
    if (it != observers_.end()) {
        observers_.erase(it);
    }
}

void Incident::notifyObservers(IncidentStatus oldStatus, IncidentStatus newStatus) {
    for (IncidentObserver* obs : observers_) {
        if (obs) {
            obs->onStatusChanged(*this, oldStatus, newStatus);
        }
    }
}

Incident& IncidentRegistry::report(IncidentType type, const std::string& location, int severity) {
    incidents_.push_back(std::unique_ptr<Incident>(new Incident(nextId_++, type, location, severity)));
    Incident& incident = *incidents_.back();
    for (IncidentObserver* obs : defaultObservers_) {
        incident.attachObserver(obs);
    }
    std::cout << "\n[Registry] Reported " << incident.label() << " (severity " << severity << ")\n";
    return incident;
}

Incident* IncidentRegistry::find(int id) {
    for (auto& incident : incidents_)
        if (incident->id() == id) return incident.get();
    return nullptr;
}

void IncidentRegistry::registerDefaultObserver(IncidentObserver* observer) {
    if (observer && std::find(defaultObservers_.begin(), defaultObservers_.end(), observer) == defaultObservers_.end()) {
        defaultObservers_.push_back(observer);
    }
}
