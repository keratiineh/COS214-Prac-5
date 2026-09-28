#include "Incident.h"
#include "IncidentObserver.h"
#include "ResponseUnits.h"
#include "Coordinator.h"
#include "Commands.h"
#include "OperatorConsole.h"
#include "CommsCentre.h"
#include "AccessControlSystem.h"
#include "LegacyAccessPanel.h"
#include "LegacyAccessAdapter.h"
#include "EmergencyOpsFacade.h"
#include "EmergencyStrategy.h"

#include <iostream>
#include <memory>

void printHeader(const std::string& title) {
    std::cout << "\n======================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "======================================================================\n";
}

int main() {
    printHeader("COS 214 Practical 5 - CampusGuard Emergency Coordination");
    std::cout << "Design Patterns: Command | Mediator | Adapter | Facade | Observer | Strategy\n";

    // -------------------------------------------------------------------------
    // Object Graph Initialization (Strict lifetime order for clean destruction)
    // -------------------------------------------------------------------------
    IncidentRegistry registry;
    CommsCentre comms;

    // Response Units (Colleagues)
    SecurityTeam security("Alpha-Security");
    MedicalTeam medical("Medic-Unit-1");
    FacilitiesTeam facilities("Facilities-Ops");

    // Legacy Access Hardware (Adaptee) & Domain Adapter (Target)
    LegacyAccessPanel legacyPanel;
    LegacyAccessAdapter accessAdapter(legacyPanel);
    facilities.setAccessControl(accessAdapter);

    // Central Coordinator (Mediator)
    CampusCoordinator coordinator(security, medical, facilities, comms);

    // Invoker (Command Console)
    OperatorConsole console("Op-Angelo");

    // Unified Workflow Entrypoint (Facade)
    EmergencyOpsFacade facade(console, security, facilities, comms);

    // Concrete Observers (Observer Pattern)
    AuditLogObserver auditLog;
    DashboardObserver dashboard("HQ-Central-Monitor");
    registry.registerDefaultObserver(&auditLog);
    registry.registerDefaultObserver(&dashboard);

    // Response Strategies (Strategy Pattern)
    ActiveThreatStrategy activeThreatStrat;
    HazardContainmentStrategy hazardStrat;
    StandardTriageStrategy triageStrat;

    // =========================================================================
    // SCENARIO 1: High-Threat Incident & Coordinated Lockdown
    // Demonstrates: Facade + Command + Adapter + Mediator + Observer + Strategy
    // =========================================================================
    printHeader("SCENARIO 1: Hostile Security Threat & Multi-Pattern Lockdown");

    std::cout << "\n[Step 1.1] Registering emergency incident in Engineering Lab 3...\n";
    Incident& inc1 = registry.report(IncidentType::SecurityThreat, "Engineering Lab 3", 4);

    std::cout << "\n[Step 1.2] Executing EmergencyOpsFacade::lockdownBuilding workflow...\n";
    std::cout << "(Facade coordinates 3 subsystem operations via Command invoker)\n";
    bool lockdownOk = facade.lockdownBuilding(inc1, "Engineering Lab 3");
    std::cout << "Facade lockdown result: " << (lockdownOk ? "SUCCESS" : "FAILED") << "\n";

    std::cout << "\n[Step 1.3] Security confirms active hostile threat on scene...\n";
    std::cout << "(Colleague reports to Mediator; Mediator escalates & coordinates colleagues)\n";
    coordinator.setStrategy(&activeThreatStrat);
    coordinator.executeStrategy(inc1);

    std::cout << "\n[Step 1.4] Security neutralizes threat; Mediator coordinates stand-down...\n";
    security.containThreat();

    std::cout << "\n[Step 1.5] Marking incident as Resolved (Observer notified)...\n";
    inc1.setStatus(IncidentStatus::Resolved);

    // =========================================================================
    // SCENARIO 2: Hazardous Chemical Incident, Medical Response & Failure Handling
    // Demonstrates: Strategy Swap + Command Undo + Graceful Failure Rejection
    // =========================================================================
    printHeader("SCENARIO 2: Chemical Hazard, Medical Response & Edge Cases");

    std::cout << "\n[Step 2.1] Registering fire/chemical incident in Chemistry Block B...\n";
    Incident& inc2 = registry.report(IncidentType::Fire, "Chemistry Block B", 3);

    std::cout << "\n[Step 2.2] Applying Hazard Containment Strategy...\n";
    coordinator.setStrategy(&hazardStrat);
    coordinator.executeStrategy(inc2);

    std::cout << "\n[Step 2.3] Demonstrating Error Handling: Invalid Duplicate Dispatch...\n";
    std::cout << "Attempting to dispatch FacilitiesTeam while already deployed to " << inc2.location() << ":\n";
    bool invalidDispatch = console.submit(
        std::unique_ptr<OperatorCommand>(new DispatchUnitCommand(facilities, inc2)));
    std::cout << "Command submission result: " << (invalidDispatch ? "ACCEPTED" : "REJECTED (as expected)") << "\n";

    std::cout << "\n[Step 2.4] Demonstrating Error Handling: Locking Already-Locked Area...\n";
    bool invalidLock = facilities.lockArea("Chemistry Block B");
    std::cout << "Lock action result: " << (invalidLock ? "ACCEPTED" : "REJECTED (as expected)") << "\n";

    std::cout << "\n[Step 2.5] Medical team arrives, treats casualties, reports to Mediator...\n";
    medical.dispatch(inc2);

    std::cout << "\n[Step 2.6] Demonstrating Command Cancellation (Undo)...\n";
    std::cout << "Operator submits an erroneous campus-wide evacuation alert:\n";
    console.submit(std::unique_ptr<OperatorCommand>(
        new IssueAlertCommand(comms, AlertLevel::Evacuate, "Chemistry Block B", "Erroneous evacuation warning.")));
    std::cout << "Current command history depth: " << console.historySize() << "\n";
    std::cout << "Operator issues cancelLast() to undo the alert:\n";
    console.cancelLast();
    std::cout << "Command history depth after undo: " << console.historySize() << "\n";

    std::cout << "\n[Step 2.7] Containing and resolving Chemistry Block incident...\n";
    facilities.unlockArea("Chemistry Block B");
    facilities.standDown();
    medical.standDown();
    inc2.setStatus(IncidentStatus::Contained);
    inc2.setStatus(IncidentStatus::Resolved);

    std::cout << "\n[Step 2.8] Demonstrating Error Handling: Modify Resolved Incident...\n";
    bool modifyResolved = inc2.setStatus(IncidentStatus::Escalated);
    std::cout << "Status change on resolved incident result: " << (modifyResolved ? "ALLOWED" : "BLOCKED (as expected)") << "\n";

    // =========================================================================
    // Audit Log Review (Observer Pattern Verification)
    // =========================================================================
    auditLog.printSummary();

    printHeader("CampusGuard Demonstration Completed Successfully");
    return 0;
}
