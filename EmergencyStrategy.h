#ifndef EMERGENCY_STRATEGY_H
#define EMERGENCY_STRATEGY_H

#include <string>

class Incident;
class FacilitiesTeam;
class MedicalTeam;
class SecurityTeam;
class CommsCentre;

//Strategy Interface
//Encapsulates incident response containment and deployment algorithms.
class EmergencyStrategy {
public:
    virtual ~EmergencyStrategy() {}
    virtual std::string name() const = 0;
    virtual bool executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                                MedicalTeam& medical, SecurityTeam& security,
                                CommsCentre& comms) = 0;
};

//ConcreteStrategy 1: High-threat armed or violent intruder protocol
class ActiveThreatStrategy : public EmergencyStrategy {
public:
    std::string name() const override { return "Active Threat Containment"; }
    bool executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                         MedicalTeam& medical, SecurityTeam& security,
                         CommsCentre& comms) override;
};

//ConcreteStrategy 2: Chemical, fire, or infrastructure hazard protocol
class HazardContainmentStrategy : public EmergencyStrategy {
public:
    std::string name() const override { return "Hazard Isolation & Containment"; }
    bool executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                         MedicalTeam& medical, SecurityTeam& security,
                         CommsCentre& comms) override;
};

//ConcreteStrategy 3: Routine medical triage and emergency transit protocol
class StandardTriageStrategy : public EmergencyStrategy {
public:
    std::string name() const override { return "Standard Medical Triage"; }
    bool executeProtocol(Incident& incident, FacilitiesTeam& facilities,
                         MedicalTeam& medical, SecurityTeam& security,
                         CommsCentre& comms) override;
};

#endif
