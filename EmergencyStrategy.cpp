#include "EmergencyStrategy.h"
#include "Incident.h"
#include "ResponseUnits.h"
#include "CommsCentre.h"

#include <iostream>

//ActiveThreatStrategy
bool ActiveThreatStrategy::executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                                           MedicalTeam& medical, SecurityTeam& security,
                                           CommsCentre& comms) {
    std::cout << "\n  [Strategy: " << name() << "] Executing active threat protocol for "
              << incident.label() << "\n";
    facilities.lockArea(incident.location());
    security.dispatch(incident);
    medical.stageNear(incident.location());
    comms.broadcast(AlertLevel::Evacuate, incident.location(),
                    "ACTIVE THREAT: Shelter in place or evacuate area immediately.");
    return true;
}

//HazardContainmentStrategy
bool HazardContainmentStrategy::executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                                                MedicalTeam& medical, SecurityTeam& /*security*/,
                                                CommsCentre& comms) {
    std::cout << "\n  [Strategy: " << name() << "] Executing hazard containment protocol for "
              << incident.label() << "\n";
    facilities.dispatch(incident);
    facilities.lockArea(incident.location());
    comms.broadcast(AlertLevel::Warning, incident.location(),
                    "HAZARD ADVISORY: Hazard condition reported. Do not enter zone.");
    if (incident.severity() >= 3) {
        medical.stageNear(incident.location());
    }
    return true;
}

//StandardTriageStrategy
bool StandardTriageStrategy::executeProtocol(Incident& incident, FacilitiesTeam& /*facilities*/,
                                             MedicalTeam& medical, SecurityTeam& security,
                                             CommsCentre& comms) {
    std::cout << "\n  [Strategy: " << name() << "] Executing standard triage protocol for "
              << incident.label() << "\n";
    medical.dispatch(incident);
    security.clearRoute(incident.location());
    comms.broadcast(AlertLevel::Info, incident.location(),
                    "Medical response underway. Yield priority transit routes.");
    return true;
}
