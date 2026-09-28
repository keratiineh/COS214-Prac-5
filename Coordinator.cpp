#include "Coordinator.h"

#include "CommsCentre.h"
#include "Incident.h"
#include "ResponseUnits.h"

#include <iostream>

CampusCoordinator::CampusCoordinator(SecurityTeam& security, MedicalTeam& medical,
                                     FacilitiesTeam& facilities, CommsCentre& comms)
    : security_(security), medical_(medical), facilities_(facilities), comms_(comms) {
    security_.setCoordinator(this);
    medical_.setCoordinator(this);
    facilities_.setCoordinator(this);
}

CampusCoordinator::~CampusCoordinator() {
    security_.setCoordinator(nullptr);
    medical_.setCoordinator(nullptr);
    facilities_.setCoordinator(nullptr);
}

void CampusCoordinator::threatConfirmed(ResponseUnit& reporter, Incident& incident) {
    log(reporter.name() + " confirmed a threat at " + incident.location() + ". Cordoning the area.");
    incident.setStatus(IncidentStatus::Escalated);
    facilities_.lockArea(incident.location());
    if (incident.severity() >= 3) medical_.stageNear(incident.location());
}

void CampusCoordinator::casualtiesReported(ResponseUnit& reporter, Incident& incident) {
    log(reporter.name() + " reported casualties at " + incident.location() + ". Arranging transport.");
    if (security_.isAvailable())
        security_.clearRoute(incident.location());
    else
        log(security_.name() + " is deployed elsewhere, no escort available");
    comms_.broadcast(AlertLevel::Info, "Campus Clinic", "Casualties inbound from " + incident.location());
}

void CampusCoordinator::areaAccessChanged(ResponseUnit& reporter, const std::string& area, bool locked) {
    log(reporter.name() + (locked ? " locked " : " unlocked ") + area + ". Informing occupants.");
    if (locked)
        comms_.broadcast(AlertLevel::Warning, area, "Area locked down. Stay clear.");
    else
        comms_.broadcast(AlertLevel::Info, area, "Area reopened.");
}

void CampusCoordinator::incidentContained(ResponseUnit& reporter, Incident& incident) {
    log(reporter.name() + " contained " + incident.label() + ". Standing the area down.");
    incident.setStatus(IncidentStatus::Contained);
    if (facilities_.isLocked(incident.location())) facilities_.unlockArea(incident.location());
    comms_.broadcast(AlertLevel::Info, incident.location(), "All clear.");
}

void CampusCoordinator::log(const std::string& message) const {
    std::cout << "  [Coordinator] " << message << "\n";
}
