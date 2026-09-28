#ifndef INCIDENT_OBSERVER_H
#define INCIDENT_OBSERVER_H

#include <string>
#include <vector>

class Incident;
enum class IncidentStatus;

//Observer interface
//Notified whenever an Incident's status changes.
class IncidentObserver {
public:
    virtual ~IncidentObserver() {}
    virtual void onStatusChanged(const Incident& incident, IncidentStatus oldStatus, IncidentStatus newStatus) = 0;
};

//ConcreteObserver 1: Audit logger that maintains an indelible audit trail
class AuditLogObserver : public IncidentObserver {
public:
    void onStatusChanged(const Incident& incident, IncidentStatus oldStatus, IncidentStatus newStatus) override;
    const std::vector<std::string>& history() const { return history_; }
    void printSummary() const;

private:
    std::vector<std::string> history_;
};

//ConcreteObserver 2: Live campus operations status board
class DashboardObserver : public IncidentObserver {
public:
    explicit DashboardObserver(const std::string& terminalName);
    void onStatusChanged(const Incident& incident, IncidentStatus oldStatus, IncidentStatus newStatus) override;

private:
    std::string terminalName_;
};

#endif
