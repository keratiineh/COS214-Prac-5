#ifndef ACCESS_CONTROL_SYSTEM_H
#define ACCESS_CONTROL_SYSTEM_H

#include <string>

//Target
/*Domain-facing interface for physical access control. FacilitiesTeam talks
to this interface only; it never sees the legacy panel directly*/
class AccessControlSystem {
    public:
        virtual ~AccessControlSystem() {}
        virtual bool lock(const std::string& area) = 0;
        virtual bool unlock(const std::string& area) = 0;
        virtual bool isLocked(const std::string& area) const = 0;
};

#endif