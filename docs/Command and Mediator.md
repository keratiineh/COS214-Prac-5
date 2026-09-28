# CampusGuard: Command and Mediator

## Responsibilities covered

- Registering incidents and tracking their operational status (`IncidentRegistry`, `Incident`).
- Operator actions as command objects: dispatch a unit, lock an area, issue an alert or evacuation instruction, and cancel the most recent action (`OperatorConsole`, `OperatorCommand` hierarchy).
- Coordination between response units when an incident changes, without units knowing about each other (`ResponseCoordinator`, `CampusCoordinator`, `ResponseUnit` hierarchy, `CommsCentre`).

## GoF participant mapping

### Command

| Role | Class |
|---|---|
| Command | `OperatorCommand` |
| ConcreteCommand | `DispatchUnitCommand`, `LockAreaCommand`, `IssueAlertCommand` |
| Invoker | `OperatorConsole` (`submit`, `cancelLast`) |
| Receiver | `ResponseUnit` subclasses, `FacilitiesTeam`, `CommsCentre` |
| Client | `main` (later the Facade, which builds commands and submits them) |

Cancelling is `undo()` on each command, driven by `OperatorConsole::cancelLast()`. This is why commands are objects: the console keeps the history and can reverse a request without knowing what kind it was.

### Mediator

| Role | Class |
|---|---|
| Mediator | `ResponseCoordinator` |
| ConcreteMediator | `CampusCoordinator` |
| Colleague | `ResponseUnit` |
| ConcreteColleague | `SecurityTeam`, `MedicalTeam`, `FacilitiesTeam` |

`CommsCentre` is a service the mediator drives. It never starts coordination, so it is not a colleague.

Coordination rules in `CampusCoordinator`:

| Event (from colleague) | Mediator reaction |
|---|---|
| `threatConfirmed` (Security) | Incident to Escalated, Facilities locks the area, Medical stages at the cordon if severity >= 3 |
| `areaAccessChanged` (Facilities) | Comms warns or reopens the zone |
| `casualtiesReported` (Medical) | Security clears a route if available, Comms alerts the Campus Clinic |
| `incidentContained` (Security) | Incident to Contained, Facilities unlocks the area, Comms sends all clear |

Why this belongs in the mediator: the rule "a confirmed threat means lock the building and stage medics" is campus policy, not Security's job. If `SecurityTeam` called `FacilitiesTeam` and `MedicalTeam` directly, every unit would depend on every other unit, and changing the policy would mean editing several colleagues. Each event is its own mediator method, so there is no switch on event type.

## Command triggering Mediator (required sequence)

`OperatorConsole::submit` -> `DispatchUnitCommand::execute` -> `SecurityTeam::dispatch` -> `CampusCoordinator::threatConfirmed` -> `FacilitiesTeam::lockArea` -> `CampusCoordinator::areaAccessChanged` -> `CommsCentre::broadcast`, plus `MedicalTeam::stageNear`.

## Ownership and destruction policy

| Owner | Owned | Mechanism |
|---|---|---|
| `IncidentRegistry` | every `Incident` | `vector<unique_ptr<Incident>>` |
| `OperatorConsole` | successfully executed commands | `vector<unique_ptr<OperatorCommand>>`; a failed command is destroyed as soon as `submit` returns; a cancelled command is destroyed after `undo()` |
| Application root (`main`, later the Facade) | units, `CommsCentre`, coordinator, console | automatic members, declared in dependency order |

Non-owning links: commands hold references to receivers; the coordinator holds references to units; units hold a `ResponseCoordinator*` and an `Incident*` assignment.

Lifetime rule: anything referenced must be declared before whatever references it. Destruction runs in reverse, so the console (and its commands) dies first, then the coordinator, which calls `setCoordinator(nullptr)` on every unit so no unit is left with a dangling pointer, then the units, then the incidents. Units, coordinator and console are non-copyable to stop accidental copies of objects that others point to.

## Failure cases handled

- Dispatching a unit that is already deployed: rejected, command discarded, not added to history.
- Locking an area that is already locked, or unlocking one that is not: rejected.
- Changing the status of a resolved incident, or dispatching to it: rejected.
- Cancelling with an empty history, or recalling a unit that has already left: reported, returns false.
- Empty alert message: rejected by `CommsCentre`.

## Integration notes for the team

- **(Facade):** the Facade can own the object graph in the same declaration order as `main`. A workflow such as `lockdownBuilding(...)` should create commands and pass them to `OperatorConsole::submit`, so the Facade is a Command client and cancellation still works. Units, comms and the console remain usable directly.
- **(Adapter):** the natural seams are `FacilitiesTeam::lockArea/unlockArea` (legacy access control) and `CommsCentre::broadcast` (external SMS gateway). Only those method bodies need to call the target interface; nothing else changes.
- **Other design patterns:** `Incident::setStatus` is the single point where status changes, which suits Observer. Unit choice in `DispatchUnitCommand` is currently explicit, which leaves room for Strategy or Factory Method.
