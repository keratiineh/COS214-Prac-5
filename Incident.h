#ifndef INCIDENT_H
#define INCIDENT_H

#include <memory>
#include <string>
#include <vector>

enum class IncidentType { Fire, Medical, SecurityThreat, Infrastructure };
enum class IncidentStatus { Reported, Dispatched, Escalated, Contained, Resolved };

std::string toString(IncidentType type);
std::string toString(IncidentStatus status);

class IncidentObserver;

class Incident {
public:
    Incident(int id, IncidentType type, const std::string& location, int severity);

    int id() const { return id_; }
    IncidentType type() const { return type_; }
    const std::string& location() const { return location_; }
    int severity() const { return severity_; }
    IncidentStatus status() const { return status_; }
    std::string label() const;

    bool setStatus(IncidentStatus next);

    // Observer / Subject methods
    void attachObserver(IncidentObserver* observer);
    void detachObserver(IncidentObserver* observer);
    void notifyObservers(IncidentStatus oldStatus, IncidentStatus newStatus);

private:
    int id_;
    IncidentType type_;
    std::string location_;
    int severity_;
    IncidentStatus status_;
    std::vector<IncidentObserver*> observers_;
};

//Sole owner of every Incident. All other classes hold non-owning references.
class IncidentRegistry {
public:
    Incident& report(IncidentType type, const std::string& location, int severity);
    Incident* find(int id);
    void registerDefaultObserver(IncidentObserver* observer);

private:
    std::vector<std::unique_ptr<Incident>> incidents_;
    std::vector<IncidentObserver*> defaultObservers_;
    int nextId_ = 1;
};

#endif
