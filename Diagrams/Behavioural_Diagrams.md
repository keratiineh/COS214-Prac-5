# CampusGuard Behavioural Diagrams (Observer & Strategy Patterns)

This document contains the required behavioural diagrams for Member 3's chosen patterns (**Observer** and **Strategy**), as required by **Task 4** and the **Team Work Split**.

---

## 1. Incident Lifecycle & Observer Notification (State Machine Diagram)

This state diagram depicts how an `Incident` transitions across its operational states and automatically triggers `notifyObservers(oldStatus, newStatus)` to update `AuditLogObserver` and `DashboardObserver`.

```mermaid
stateDiagram-v2
        [*] --> Reported: IncidentRegistry.report()

    Reported --> Dispatched: ResponseUnit.assign()\n[notifyObservers: Dispatched]

    Dispatched --> Escalated: CampusCoordinator.threatConfirmed()\n[notifyObservers: Escalated]

    Escalated --> Contained: CampusCoordinator.incidentContained()\n[notifyObservers: Contained]
    
    Dispatched --> Contained: Security / Hazard resolved\n[notifyObservers: Contained]
    
    Contained --> Resolved: All units stand down & area reopened\n[notifyObservers: Resolved]
    
    Resolved --> [*]

    note right of Reported
        Default observers attached:
        - AuditLogObserver
        - DashboardObserver
    end note

    note right of Escalated
        Triggers emergency strategy execution
        (e.g., ActiveThreatStrategy)
    end note

    note right of Resolved
        Terminal state:
        Any further setStatus() calls are REJECTED
    end note
```

---

## 2. Emergency Strategy Execution Workflow (Sequence Diagram)

This sequence diagram illustrates how `CampusCoordinator` delegates incident handling to an `EmergencyStrategy` (`ActiveThreatStrategy`), coordinating `FacilitiesTeam` (via `LegacyAccessAdapter`), `SecurityTeam`, `MedicalTeam`, and `CommsCentre` without procedural switch statements.

```mermaid
sequenceDiagram
    autonumber
    actor Operator as Operator
    participant Facade as EmergencyOpsFacade
    participant Console as OperatorConsole
    participant Coord as CampusCoordinator
    participant Strat as ActiveThreatStrategy
    participant Fac as FacilitiesTeam
    participant Adapter as LegacyAccessAdapter
    participant Panel as LegacyAccessPanel
    participant Sec as SecurityTeam
    participant Med as MedicalTeam
    participant Comms as CommsCentre
    participant Inc as Incident
    participant Obs as IncidentObserver (AuditLog/Dashboard)

    Operator->>Facade: lockdownBuilding(incident, "Engineering Lab 3")
    
    %% Step 1: Facilities Lock Command
    Facade->>Console: submit(LockAreaCommand)
    Console->>Fac: lockArea("Engineering Lab 3")
    Fac->>Adapter: lock("Engineering Lab 3")
    Adapter->>Panel: engageLock(zoneCode=100)
    Panel-->>Adapter: STATUS_OK (0)
    Adapter-->>Fac: true
    Fac-->>Console: true
    
    %% Step 2: Security Dispatch Command
    Facade->>Console: submit(DispatchUnitCommand)
    Console->>Sec: dispatch(incident)
    Sec->>Inc: setStatus(Dispatched)
    Inc->>Obs: onStatusChanged(Reported -> Dispatched)
    Sec->>Sec: threatConfirmed on scene
    Sec->>Coord: threatConfirmed(reporter, incident)
    
    %% Step 3: Mediator Strategy Delegation
    Coord->>Coord: setStrategy(ActiveThreatStrategy)
    Coord->>Coord: executeStrategy(incident)
    Coord->>Strat: executeProtocol(incident, facilities, medical, security, comms)
    
    Strat->>Med: stageNear("Engineering Lab 3")
    Strat->>Comms: broadcast(Evacuate, "Engineering Lab 3", "Active threat...")
    
    %% Neutralization & Resolution
    Sec->>Coord: incidentContained(reporter, incident)
    Coord->>Inc: setStatus(Contained)
    Inc->>Obs: onStatusChanged(Escalated -> Contained)
    Coord->>Fac: unlockArea("Engineering Lab 3")
    Fac->>Adapter: unlock("Engineering Lab 3")
    Adapter->>Panel: releaseLock(zoneCode=100)
    Coord->>Comms: broadcast(Info, "All clear")
```

---

## 3. Emergency Protocol Selection & Triage (Activity Diagram)

```mermaid
flowchart TD
    Start([Incident Reported]) --> Classify[Evaluate Incident Type & Severity]
    
    Classify --> Condition{Threat Type?}
    
    Condition -->|Violent / Hostile Intruder| S1[Select ActiveThreatStrategy]
    Condition -->|Chemical / Fire / Gas Leak| S2[Select HazardContainmentStrategy]
    Condition -->|Casualty / Routine Medical| S3[Select StandardTriageStrategy]
    
    S1 --> P1[Lock perimeter doors via Legacy Adapter]
    P1 --> P2[Dispatch armed security unit]
    P2 --> P3[Stage paramedics outside cordon]
    P3 --> P4[Broadcast EVACUATE siren]
    
    S2 --> H1[Dispatch facilities for HVAC isolation]
    H1 --> H2[Lock affected laboratory wing]
    H2 --> H3[Broadcast HAZARD WARNING]
    H3 --> H4[Check Severity >= 3: Stage Decontamination Medics]
    
    S3 --> M1[Dispatch medical team to victim]
    M1 --> M2[Security clears transit corridor]
    M2 --> M3[Broadcast traffic advisory]
    
    P4 --> Notify[Incident::setStatus updates attached Observers]
    H4 --> Notify
    M3 --> Notify
    
    Notify --> Audit[AuditLogObserver logs immutable record]
    Notify --> Dash[DashboardObserver updates live status board]
    Audit --> End([Protocol Deployed])
    Dash --> End
```
