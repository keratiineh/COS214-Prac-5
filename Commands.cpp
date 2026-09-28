#include "Commands.h"

#include "Incident.h"
#include "ResponseUnits.h"

#include <iostream>

//DispatchUnitCommand

DispatchUnitCommand::DispatchUnitCommand(ResponseUnit& unit, Incident& incident)
    : unit_(unit), incident_(incident) {}

bool DispatchUnitCommand::execute() { return unit_.dispatch(incident_); }

bool DispatchUnitCommand::undo() {
    if (!unit_.isAssignedTo(incident_)) {
        std::cout << "  [Command] " << unit_.name() << " is no longer on " << incident_.label()
                  << ", nothing to recall\n";
        return false;
    }
    return unit_.standDown();
}

std::string DispatchUnitCommand::describe() const {
    return "Dispatch " + unit_.name() + " to " + incident_.label();
}

//LockAreaCommand

LockAreaCommand::LockAreaCommand(FacilitiesTeam& facilities, const std::string& area)
    : facilities_(facilities), area_(area) {}

bool LockAreaCommand::execute() { return facilities_.lockArea(area_); }
bool LockAreaCommand::undo() { return facilities_.unlockArea(area_); }
std::string LockAreaCommand::describe() const { return "Lock " + area_; }

//IssueAlertCommand

IssueAlertCommand::IssueAlertCommand(CommsCentre& comms, AlertLevel level, const std::string& zone,
                                     const std::string& message)
    : comms_(comms), level_(level), zone_(zone), message_(message) {}

bool IssueAlertCommand::execute() { return comms_.broadcast(level_, zone_, message_); }
bool IssueAlertCommand::undo() { return comms_.retract(zone_); }

std::string IssueAlertCommand::describe() const {
    return "Issue " + toString(level_) + " alert to " + zone_;
}
