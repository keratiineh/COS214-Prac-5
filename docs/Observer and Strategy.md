# CampusGuard: Observer and Strategy

## Responsibilities Covered

- Real-time decoupling of incident status transitions from auditing and operational monitoring (`IncidentObserver`, `Incident`, `AuditLogObserver`, `DashboardObserver`).
- Dynamic encapsulation of threat-containment and emergency-response algorithms without procedural switch statements or if/else ladders (`EmergencyStrategy`, `ActiveThreatStrategy`, `HazardContainmentStrategy`, `StandardTriageStrategy`).
- Runtime coordination of emergency response protocols driven through `CampusCoordinator`.

---

## GoF Participant Mapping

### 1. Observer Pattern

| Role | Class | Description |
|---|---|---|
| **Subject Interface / Host** | `Incident` | Maintains a list of attached observers and triggers `notifyObservers()` whenever `setStatus()` is invoked. |
| **Observer** | `IncidentObserver` | Polymorphic base class defining `virtual void onStatusChanged(...)` with a virtual destructor. |
| **ConcreteObserver 1** | `AuditLogObserver` | Appends chronological, timestamped records of all incident state transitions for historical and compliance verification. |
| **ConcreteObserver 2** | `DashboardObserver` | Terminal display component notifying operational operators immediately of campus state updates. |
| **Client / Owner** | Application root (`main`, `IncidentRegistry`) | Observers are allocated with automatic lifetime in `main` and registered with `IncidentRegistry` or individual incidents. |

#### Why Observer Over Simpler Alternatives
Without Observer, logging and operator alerting would need to be manually hardcoded inside `Incident::setStatus()`, or every caller updating an incident would need to remember to call audit log and dashboard functions manually. This would tightly couple `Incident` (a core domain entity) to display and logging infrastructure, violating both the **Single Responsibility Principle** and the **Open-Closed Principle**.

The Observer pattern allows new observers (such as remote alert gateways, analytics collectors, or database recorders) to be attached at runtime without touching a single line of `Incident` code.

---

### 2. Strategy Pattern

| Role | Class | Description |
|---|---|---|
| **Strategy** | `EmergencyStrategy` | Polymorphic base class declaring `executeProtocol(Incident&, FacilitiesTeam&, MedicalTeam&, SecurityTeam&, CommsCentre&)` with a virtual destructor. |
| **ConcreteStrategy 1** | `ActiveThreatStrategy` | Enforces lockdown on perimeter doors, deploys armed security, stages medics outside the cordon, and broadcasts an evacuation / shelter-in-place alarm. |
| **ConcreteStrategy 2** | `HazardContainmentStrategy` | Dispatches facilities for HVAC / utility isolation, locks affected rooms, issues hazard warnings, and stages medical decontamination. |
| **ConcreteStrategy 3** | `StandardTriageStrategy` | Dispatches medical teams directly, commands security to clear emergency transit routes, and broadcasts traffic advisories. |
| **Context** | `CampusCoordinator` | Holds a non-owning pointer to the current `EmergencyStrategy` and delegates emergency response protocol execution via `executeStrategy()`. |

#### Why Strategy Over Simpler Alternatives
Without Strategy, `CampusCoordinator` or `EmergencyOpsFacade` would require large conditional blocks (`switch(incident.type())` or chains of `if/else`) checking incident types and severity to decide what steps to take. Non-negotiable Rule 8 strictly prohibits replacing patterns with large centralized if/else chains or switch statements.

The Strategy pattern encapsulates each emergency containment algorithm into its own cohesive class, allowing new protocols to be added easily and enabling the coordinator to swap strategies dynamically at runtime as threat levels evolve.

---

## Pattern Collaboration in CampusGuard

The Observer and Strategy patterns participate seamlessly in the integrated system:
1. `EmergencyOpsFacade` or `OperatorConsole` initiates emergency action.
2. `CampusCoordinator` sets and applies an `EmergencyStrategy` (e.g., `ActiveThreatStrategy`).
3. The strategy commands `FacilitiesTeam::lockArea()` (which translates via `LegacyAccessAdapter` to `LegacyAccessPanel`) and `SecurityTeam::dispatch()`.
4. `SecurityTeam` arrives and confirms an escalated threat $\to$ `Incident::setStatus(IncidentStatus::Escalated)`.
5. `Incident::setStatus()` invokes `notifyObservers()`, instantly updating `AuditLogObserver` and `DashboardObserver`.
6. Once the threat is contained, the coordinator resets access and updates the incident status, producing a full audit trail.

---

## Ownership and Destruction Policy

| Owner | Owned Object | Ownership Mechanism | Notes |
|---|---|---|---|
| Application Root (`main`) | `AuditLogObserver`, `DashboardObserver` | Automatic stack allocation | Observers outlive all subject incidents. |
| Application Root (`main`) | Concrete strategies (`ActiveThreatStrategy`, etc.) | Automatic stack allocation | Strategies outlive the coordinator. |
| `Incident` | Observers | Non-owning pointers (`std::vector<IncidentObserver*>`) | Clear separation of lifetime: incidents never delete observers. |
| `CampusCoordinator` | Current strategy | Non-owning pointer (`EmergencyStrategy*`) | Swappable at runtime; does not manage strategy memory. |
