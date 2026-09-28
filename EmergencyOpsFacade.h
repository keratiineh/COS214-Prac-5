#ifndef EMERGENCY_OPS_FACADE_H
#define EMERGENCY_OPS_FACADE_H

#include <string>

class OperatorConsole;
class SecurityTeam;
class FacilitiesTeam;
class CommsCentre;
class Incident;

//Facade
/*Single higher-level entry point for a realistic multi-step emergency
workflow. Builds concrete Commands and submits them through the existing
OperatorConsole invoker, so cancellation still works and every subsystem
operation it touches remains independently usable outside the facade*/
class EmergencyOpsFacade {
    private:
        OperatorConsole& console;
        SecurityTeam& security;
        FacilitiesTeam& facilities;
        CommsCentre& comms;

    public:
        EmergencyOpsFacade(OperatorConsole& console, SecurityTeam& security,
                        FacilitiesTeam& facilities, CommsCentre& comms);

        /*Coordinates 3+ subsystem operations in one call: dispatch security,
        lock the area (via the access-control adapter), and broadcast a
        warning through comms*/
        bool lockdownBuilding(Incident& incident, const std::string& area);

};

#endif