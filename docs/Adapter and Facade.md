# CampusGuard: Adapter and Facade

## Responsibilities covered

- Integrating a legacy, incompatible access-control system into CampusGuard without changing the domain-facing interface `FacilitiesTeam` and the rest of the app already depend on (`AccessControlSystem`, `LegacyAccessPanel`, `LegacyAccessAdapter`).
- Providing a single, higher-level workflow entry point for a realistic multi-step emergency response, so a client doesn't need to know about or manually sequence three separate subsystem calls (`EmergencyOpsFacade`).

## GoF participant mapping

### Adapter

| Role | Class |
|---|---|
| Target | `AccessControlSystem` |
| Adaptee | `LegacyAccessPanel` |
| Adapter | `LegacyAccessAdapter` |
| Client | `FacilitiesTeam` (via `access_`), later the Facade indirectly through `FacilitiesTeam` |

This is an **Object Adapter** (composition, not inheritance): `LegacyAccessAdapter` holds a reference to `LegacyAccessPanel` rather than inheriting from it, so the panel can be swapped or mocked independently for testing.

### Facade

| Role | Class |
|---|---|
| Facade | `EmergencyOpsFacade` |
| Subsystem classes | `SecurityTeam`, `FacilitiesTeam`, `CommsCentre` (via `OperatorConsole` as invoker) |
| Client | `main` |

## Why Adapter

`LegacyAccessPanel` represents an existing campus door-control system. It speaks entirely in numeric zone codes and integer status codes (`engageLock(int) -> int`), with no concept of area names or booleans. Every other part of CampusGuard, including `FacilitiesTeam::lockArea(const std::string&)` and the UML already agreed on by the team, works in area names and simple `bool` results.

`LegacyAccessAdapter` is a genuine translation, not a thin wrapper:
- it maintains a `std::map<std::string, int>` so area names get looked up (or lazily assigned) a zone code the panel understands;
- it converts the panel's integer status codes (`STATUS_OK`, `STATUS_ALREADY_ENGAGED`, etc.) back into the `bool` result the rest of the app expects.

**Why Adapter over a simpler alternative:** we could have modified `LegacyAccessPanel` directly to accept strings and return bools, but that is not realistic: a legacy/external system's interface is, by definition, not ours to change. Adapter lets us keep the panel exactly as it is and isolate all translation logic in one class, so if the legacy system is ever replaced, only `LegacyAccessAdapter` needs to change.

## Why Facade

Locking down a building during an incident is not one action; it is dispatching a response unit, locking the physical area, and issuing a warning: three separate subsystem calls that must happen together and in a sensible order. Without a Facade, any client wanting this behaviour would need to know about `SecurityTeam`, `FacilitiesTeam`, `CommsCentre`, and `OperatorConsole` individually, and would need to remember to sequence all three commands correctly every time.

`EmergencyOpsFacade::lockdownBuilding(Incident&, const std::string&)` hides that sequencing behind one call. It builds and submits a `DispatchUnitCommand`, `LockAreaCommand`, and `IssueAlertCommand` through the existing `OperatorConsole` invoker, so:
- cancellation (`OperatorConsole::cancelLast()`) still works on each individual step, since the Facade is a Command **client**, not a replacement for Command;
- every subsystem operation the Facade touches (`SecurityTeam::dispatch`, `FacilitiesTeam::lockArea`, `CommsCentre::broadcast`) remains independently callable outside the Facade, satisfying the requirement that Facade must not prevent direct subsystem access.

**Why Facade over a simpler alternative:** we could have left this sequencing as ad-hoc code wherever a lockdown is triggered (e.g. duplicated in `main` and in the mediator), but that spreads the same three-step recipe across the codebase and makes it easy for one caller to forget a step. Facade centralises the recipe in one place while still using Command underneath, so nothing about the invoker/undo behaviour is bypassed.

## Adapter + Facade interaction (for the demo)

`EmergencyOpsFacade::lockdownBuilding()` -> `OperatorConsole::submit(LockAreaCommand)` -> `LockAreaCommand::execute()` -> `FacilitiesTeam::lockArea(area)` -> `LegacyAccessAdapter::lock(area)` -> `LegacyAccessPanel::engageLock(zoneCode)`.

This is one runtime flow that involves Facade, Command, and Adapter together, which satisfies the practical's requirement that at least one scenario show four or more of the six patterns collaborating (Command and Mediator already meet at `DispatchUnitCommand` -> `SecurityTeam::dispatch` -> `CampusCoordinator::threatConfirmed` in the same story).

## Ownership and destruction policy

| Owner | Owned | Mechanism |
|---|---|---|
| Application root (`main`) | `LegacyAccessPanel`, `LegacyAccessAdapter`, `EmergencyOpsFacade` | automatic members, declared in dependency order (panel before adapter, since the adapter holds a reference to it) |
| `FacilitiesTeam` | nothing new | holds a non-owning `AccessControlSystem*`, set via `setAccessControl()`; defaults to `nullptr` so the unit still works standalone with its own local `locked_` set if no adapter is attached |
| `EmergencyOpsFacade` | nothing | holds non-owning references to `OperatorConsole`, `SecurityTeam`, `FacilitiesTeam`, `CommsCentre`; all must outlive the Facade |

Lifetime rule: the panel must be declared before the adapter that references it, and the adapter must be attached to `FacilitiesTeam` (via `setAccessControl`) before any lockdown workflow runs.

## Failure cases handled

- Locking an area that is already locked, or unlocking one that isn't: the legacy panel reports `STATUS_ALREADY_ENGAGED` / `STATUS_ALREADY_RELEASED`, which the adapter translates to `false` rather than silently succeeding.
- An unrecognised zone code: reported as `STATUS_UNKNOWN_ZONE`, translated to `false`.
- No adapter attached: `FacilitiesTeam` falls back to its own local lock-tracking rather than crashing on a null pointer.

## Integration notes for the team

- **(Command/Mediator):** `LockAreaCommand` already calls `FacilitiesTeam::lockArea()` as its receiver action, so once the adapter is attached, locking through the Command pattern automatically reaches the legacy system with no changes needed to `LockAreaCommand` itself.
- **Pending team sign-off:** `FacilitiesTeam` needs one small addition to accept the adapter (`setAccessControl()` plus a private `AccessControlSystem*`). This does not change any existing constructor or method signature.