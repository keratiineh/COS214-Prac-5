#ifndef LEGACY_ACCESS_ADAPTER_H
#define LEGACY_ACCESS_ADAPTER_H

#include "AccessControlSystem.h"
#include "LegacyAccessPanel.h"

#include <map>
#include <string>

//Adapter
/*Translates the domain-facing AccessControlSystem interface (area names,
bool results) into the calls the LegacyAccessPanel actually understands
(zone codes, integer status codes). The panel is referenced, never owned;
it must outlive the adapter*/
class LegacyAccessAdapter : public AccessControlSystem {
    private:
        /*Looks up (or lazily assigns) the zone code for an area name. Marked
        for use from const methods since isLocked() needs the same mapping*/
        int zoneCodeFor(const std::string& area) const;

        LegacyAccessPanel& panel;
        mutable std::map<std::string, int> zoneCodes;
        mutable int nextZoneCode = 100;
        
    public:
        explicit LegacyAccessAdapter(LegacyAccessPanel& panel);

        bool lock(const std::string& area) override;
        bool unlock(const std::string& area) override;
        bool isLocked(const std::string& area) const override;
};

#endif