#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <string>

class Incident;
class ResponseUnit;
class SecurityTeam;
class MedicalTeam;
class FacilitiesTeam;
class CommsCentre;

//Mediator
//Response units report events here instead of calling each other.
class ResponseCoordinator {
public:
    virtual ~ResponseCoordinator() {}
    virtual void threatConfirmed(ResponseUnit& reporter, Incident& incident) = 0;
    virtual void casualtiesReported(ResponseUnit& reporter, Incident& incident) = 0;
    virtual void areaAccessChanged(ResponseUnit& reporter, const std::string& area, bool locked) = 0;
    virtual void incidentContained(ResponseUnit& reporter, Incident& incident) = 0;
};

class EmergencyStrategy;

//Concrete mediator
//Holds non-owning references; attaches itself to the
//units on construction and detaches on destruction.
class CampusCoordinator : public ResponseCoordinator {
public:
    CampusCoordinator(SecurityTeam& security, MedicalTeam& medical,
                      FacilitiesTeam& facilities, CommsCentre& comms);
    ~CampusCoordinator() override;

    CampusCoordinator(const CampusCoordinator&) = delete;
    CampusCoordinator& operator=(const CampusCoordinator&) = delete;

    void threatConfirmed(ResponseUnit& reporter, Incident& incident) override;
    void casualtiesReported(ResponseUnit& reporter, Incident& incident) override;
    void areaAccessChanged(ResponseUnit& reporter, const std::string& area, bool locked) override;
    void incidentContained(ResponseUnit& reporter, Incident& incident) override;

    void setStrategy(EmergencyStrategy* strategy) { strategy_ = strategy; }
    EmergencyStrategy* strategy() const { return strategy_; }
    bool executeStrategy(Incident& incident);

private:
    void log(const std::string& message) const;

    SecurityTeam& security_;
    MedicalTeam& medical_;
    FacilitiesTeam& facilities_;
    CommsCentre& comms_;
    EmergencyStrategy* strategy_ = nullptr; // non-owning
};

#endif
