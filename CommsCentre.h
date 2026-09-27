#ifndef COMMS_CENTRE_H
#define COMMS_CENTRE_H

#include <string>

enum class AlertLevel { Info, Warning, Evacuate };

std::string toString(AlertLevel level);

//Campus-wide messaging service. Receiver for IssueAlertCommand and a
//service the coordinator uses. It never initiates coordination itself.
class CommsCentre {
public:
    bool broadcast(AlertLevel level, const std::string& zone, const std::string& message);
    bool retract(const std::string& zone);
};

#endif
