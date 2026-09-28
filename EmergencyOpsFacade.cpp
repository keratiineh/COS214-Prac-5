#include "EmergencyOpsFacade.h"
#include "OperatorConsole.h"
#include "ResponseUnits.h"
#include "CommsCentre.h"
#include "Commands.h"
#include "Incident.h"

#include <memory>

bool EmergencyOpsFacade::lockdownBuilding(Incident& incident, const std::string& area) {
    /*Three subsystem operations, coordinated in one call, each still going
    through the existing Command/Invoker path so cancellation still works
    and nothing here bypasses the rest of the team's design*/
    bool dispatched = console.submit(
        std::unique_ptr<OperatorCommand>(new DispatchUnitCommand(security, incident)));

    bool locked = console.submit(
        std::unique_ptr<OperatorCommand>(new LockAreaCommand(facilities, area)));

    bool alerted = console.submit(
        std::unique_ptr<OperatorCommand>(new IssueAlertCommand(
            comms, AlertLevel::Warning, area,
            "Lockdown in progress: " + area)));

    return dispatched && locked && alerted;
}