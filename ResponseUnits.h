#ifndef RESPONSE_UNITS_H
#define RESPONSE_UNITS_H

#include <set>
#include <string>

class Incident;
class ResponseCoordinator;
class AccessControlSystem;

//Colleague base
//Knows only the ResponseCoordinator interface, never other units.
//Works standalone (no coordinator attached) so units stay independently usable.
class ResponseUnit {
public:
    explicit ResponseUnit(const std::string& name);
    virtual ~ResponseUnit() {}

    ResponseUnit(const ResponseUnit&) = delete;
    ResponseUnit& operator=(const ResponseUnit&) = delete;

    virtual bool dispatch(Incident& incident) = 0;
    bool standDown();

    void setCoordinator(ResponseCoordinator* coordinator) { coordinator_ = coordinator; }
    const std::string& name() const { return name_; }
    bool isAvailable() const { return assignment_ == nullptr; }
    bool isAssignedTo(const Incident& incident) const { return assignment_ == &incident; }

protected:
    bool assign(Incident& incident);
    Incident* assignment() const { return assignment_; }
    ResponseCoordinator* coordinator() const { return coordinator_; }
    void log(const std::string& message) const;

private:
    std::string name_;
    ResponseCoordinator* coordinator_ = nullptr;  // non-owning
    Incident* assignment_ = nullptr;              // non-owning, owned by IncidentRegistry
};

class SecurityTeam : public ResponseUnit {
public:
    explicit SecurityTeam(const std::string& name) : ResponseUnit(name) {}
    bool dispatch(Incident& incident) override;
    bool containThreat();
    void clearRoute(const std::string& area);
};

class MedicalTeam : public ResponseUnit {
public:
    explicit MedicalTeam(const std::string& name) : ResponseUnit(name) {}
    bool dispatch(Incident& incident) override;
    void stageNear(const std::string& area);
};

class FacilitiesTeam : public ResponseUnit {
public:
    explicit FacilitiesTeam(const std::string& name) : ResponseUnit(name) {}
    bool dispatch(Incident& incident) override;
    bool lockArea(const std::string& area);
    bool unlockArea(const std::string& area);
    bool isLocked(const std::string& area) const;
    //lets the Facade/adapter attach itself to this unit, the same way setCoordinator() already works for the Mediator
    void setAccessControl(AccessControlSystem& access) { access_ = &access; }
private:
    std::set<std::string> locked_;
    AccessControlSystem* access_ = nullptr; //non-owning, may be null if no adapter is attached yet
};

#endif
