#include "ResponseUnits.h"

#include "Coordinator.h"
#include "Incident.h"
#include "AccessControlSystem.h"

#include <iostream>

//ResponseUnit

ResponseUnit::ResponseUnit(const std::string& name) : name_(name) {}

bool ResponseUnit::assign(Incident& incident) {
    if (assignment_) {
        log("REJECTED: already deployed to " + assignment_->label());
        return false;
    }
    if (incident.status() == IncidentStatus::Resolved) {
        log("REJECTED: " + incident.label() + " is already resolved");
        return false;
    }
    assignment_ = &incident;
    log("deployed to " + incident.label());
    if (incident.status() == IncidentStatus::Reported) incident.setStatus(IncidentStatus::Dispatched);
    return true;
}

bool ResponseUnit::standDown() {
    if (!assignment_) {
        log("REJECTED: stand-down requested but unit is not deployed");
        return false;
    }
    log("standing down from " + assignment_->label());
    assignment_ = nullptr;
    return true;
}

void ResponseUnit::log(const std::string& message) const {
    std::cout << "  [" << name_ << "] " << message << "\n";
}

//SecurityTeam

bool SecurityTeam::dispatch(Incident& incident) {
    if (!assign(incident)) return false;
    bool threat = incident.type() == IncidentType::SecurityThreat || incident.type() == IncidentType::Fire;
    if (threat) {
        log("active threat confirmed on scene");
        if (coordinator()) coordinator()->threatConfirmed(*this, incident);
    }
    return true;
}

bool SecurityTeam::containThreat() {
    Incident* incident = assignment();
    if (!incident) {
        log("REJECTED: no active incident to contain");
        return false;
    }
    log("threat neutralised at " + incident->location());
    if (coordinator()) coordinator()->incidentContained(*this, *incident);
    return standDown();
}

void SecurityTeam::clearRoute(const std::string& area) {
    log("clearing an emergency route to " + area);
}

//MedicalTeam

bool MedicalTeam::dispatch(Incident& incident) {
    if (!assign(incident)) return false;
    if (incident.severity() >= 3) {
        log("multiple casualties, requesting transport");
        if (coordinator()) coordinator()->casualtiesReported(*this, incident);
    } else {
        log("treating on scene");
    }
    return true;
}

void MedicalTeam::stageNear(const std::string& area) {
    log("staging at the cordon outside " + area);
}

//FacilitiesTeam

bool FacilitiesTeam::dispatch(Incident& incident) {
    if (!assign(incident)) return false;
    log("inspecting building services at " + incident.location());
    return true;
}

bool FacilitiesTeam::isLocked(const std::string& area) const {
    return access_ ? access_->isLocked(area) : locked_.count(area) > 0;
}

bool FacilitiesTeam::lockArea(const std::string& area) {
    if (isLocked(area)) {
        log("REJECTED: " + area + " is already locked");
        return false;
    }
    if (access_) {
        if (!access_->lock(area)) {
            log("FAILED: access control could not lock " + area);
            return false;
        }
    } else {
        locked_.insert(area);
    }
    log("locked " + area);
    if (coordinator()) coordinator()->areaAccessChanged(*this, area, true);
    return true;
}

bool FacilitiesTeam::unlockArea(const std::string& area) {
    if (!isLocked(area)) {
        log("REJECTED: " + area + " is not locked");
        return false;
    }
    if (access_) {
        if (!access_->unlock(area)) {
            log("FAILED: access control could not unlock " + area);
            return false;
        }
    } else {
        locked_.erase(area);
    }
    log("unlocked " + area);
    if (coordinator()) coordinator()->areaAccessChanged(*this, area, false);
    return true;
}
