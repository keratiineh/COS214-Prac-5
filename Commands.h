#ifndef COMMANDS_H
#define COMMANDS_H

#include "CommsCentre.h"

#include <string>

class Incident;
class ResponseUnit;
class FacilitiesTeam;

//Command
//Receivers are referenced, never owned, and must outlive the command.
class OperatorCommand {
public:
    virtual ~OperatorCommand() {}
    virtual bool execute() = 0;
    virtual bool undo() = 0;
    virtual std::string describe() const = 0;
};

class DispatchUnitCommand : public OperatorCommand {
public:
    DispatchUnitCommand(ResponseUnit& unit, Incident& incident);
    bool execute() override;
    bool undo() override;
    std::string describe() const override;

private:
    ResponseUnit& unit_;
    Incident& incident_;
};

class LockAreaCommand : public OperatorCommand {
public:
    LockAreaCommand(FacilitiesTeam& facilities, const std::string& area);
    bool execute() override;
    bool undo() override;
    std::string describe() const override;

private:
    FacilitiesTeam& facilities_;
    std::string area_;
};

class IssueAlertCommand : public OperatorCommand {
public:
    IssueAlertCommand(CommsCentre& comms, AlertLevel level, const std::string& zone,
                      const std::string& message);
    bool execute() override;
    bool undo() override;
    std::string describe() const override;

private:
    CommsCentre& comms_;
    AlertLevel level_;
    std::string zone_;
    std::string message_;
};

#endif
