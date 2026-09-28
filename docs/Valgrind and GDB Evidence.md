# CampusGuard: Valgrind and GDB Evidence

This document contains the engineering quality verification evidence for Task 5, including memory safety verification with Valgrind and a step-by-step GDB investigation of a real execution interaction bug.

---

## 1. Valgrind Memory Leak Audit

The final executable was built with `make` using `-std=c++11 -Wall -Wextra -Wpedantic -g` and tested with:
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./campus_guard
```

### Valgrind Output Summary:
```
==498640== Memcheck, a memory error detector
==498640== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==498640== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==498640== Command: ./campus_guard
==498640== 
... (Execution of Scenario 1 and Scenario 2) ...
==498640== 
==498640== HEAP SUMMARY:
==498640==     in use at exit: 0 bytes in 0 blocks
==498640==   total heap usage: 192 allocs, 192 frees, 84,497 bytes allocated
==498640== 
==498640== All heap blocks were freed -- no leaks are possible
==498640== 
==498640== For lists of detected and suppressed errors, rerun with: -s
==498640== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Analysis of Memory Policy:
1. **0 bytes lost in 0 blocks**: Perfect memory management across dynamic allocations (`std::unique_ptr` in `IncidentRegistry`, `std::unique_ptr` in `OperatorConsole` command history).
2. **Polymorphic Destructors**: All polymorphic base classes (`OperatorCommand`, `ResponseUnit`, `ResponseCoordinator`, `AccessControlSystem`, `IncidentObserver`, `EmergencyStrategy`) have virtual destructors, ensuring clean deallocation when destroyed through base pointers.
3. **No Dangling Pointers**: Detach methods (`Coordinator::~CampusCoordinator()`, `Incident::detachObserver()`) guarantee non-owning references are safely cleared.

---

## 2. GDB Investigation of a Real Execution Bug

### Problem Description
During early integration of `EmergencyOpsFacade::lockdownBuilding` with `CampusCoordinator` and `SecurityTeam`, the lockdown workflow reported `Facade lockdown result: FAILED` instead of succeeding.

### Debugging with GDB
To diagnose the failure point, breakpoints were placed in `EmergencyOpsFacade::lockdownBuilding` and `CampusCoordinator::threatConfirmed`:

```bash
gdb ./campus_guard
(gdb) b main
Breakpoint 1 at 0x16fe0: file main.cpp, line 30.
(gdb) b EmergencyOpsFacade::lockdownBuilding
Breakpoint 2 at 0x98f0: file EmergencyOpsFacade.cpp, line 14.
(gdb) b CampusCoordinator::threatConfirmed
Breakpoint 3 at 0xa4d0: file Coordinator.cpp, line 23.
(gdb) run
```

### Callstack Trace & Variable Inspection:
```
Breakpoint 3, CampusCoordinator::threatConfirmed (this=0x7fffffffd000, reporter=..., incident=...) at Coordinator.cpp:23
23	void CampusCoordinator::threatConfirmed(ResponseUnit& reporter, Incident& incident) {
(gdb) bt
#0  CampusCoordinator::threatConfirmed (this=0x7fffffffd000, reporter=..., incident=...) at Coordinator.cpp:23
#1  0x0000555555564731 in SecurityTeam::dispatch (this=0x7fffffffd130, incident=...) at ResponseUnits.cpp:49
#2  0x00005555555576b2 in DispatchUnitCommand::execute (this=0x5555555869d0) at Commands.cpp:13
#3  0x00005555555628f1 in OperatorConsole::submit (this=0x7fffffffd1b0, command=std::unique_ptr<OperatorCommand> = {...}) at OperatorConsole.cpp:9
#4  0x0000555555559985 in EmergencyOpsFacade::lockdownBuilding (this=0x7fffffffcfc0, incident=..., area="Engineering Lab 3") at EmergencyOpsFacade.cpp:21
#5  0x00005555555670fd in main () at main.cpp:74
```

### Root Cause Identification:
1. When `EmergencyOpsFacade::lockdownBuilding` dispatched `SecurityTeam` first, `SecurityTeam::dispatch` confirmed the threat and immediately invoked `CampusCoordinator::threatConfirmed`.
2. Inside `threatConfirmed`, the mediator called `facilities_.lockArea(incident.location())` unconditionally.
3. When the second command of the Facade (`LockAreaCommand`) was subsequently submitted to `FacilitiesTeam`, the area was already locked!
4. `FacilitiesTeam::lockArea()` returned `false` ("REJECTED: already locked"), causing `OperatorConsole::submit` to discard the command as failed, which broke the atomic lockdown sequence.

### Resolution:
1. Added an `isLocked` guard inside `CampusCoordinator::threatConfirmed`:
   ```cpp
   if (!facilities_.isLocked(incident.location())) {
       facilities_.lockArea(incident.location());
   }
   ```
2. Re-sequenced `EmergencyOpsFacade::lockdownBuilding` to secure access control first, dispatch response teams second, and broadcast alerts third—matching real-world emergency protocol precedence.
3. Verified fix with GDB: All three commands executed with return status `true`, and the Facade succeeded.
