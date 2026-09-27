#ifndef LEGACY_ACCESS_PANEL_H
#define LEGACY_ACCESS_PANEL_H

//Adaptee
/*Pre-existing campus door-control hardware. Speaks in numeric zone codes
and integer status codes; has no concept of area names or booleans, and
was never designed with CampusGuard in mind*/
class LegacyAccessPanel {
    private:
        /*Simulated hardware state for demo; a real panel would talk to
        actual door controllers here instead*/
        bool engagedZones[256] = {};
        
    public:
        static const int STATUS_OK = 0;
        static const int STATUS_ALREADY_ENGAGED = 1;
        static const int STATUS_ALREADY_RELEASED = 2;
        static const int STATUS_UNKNOWN_ZONE = 3;

        /*Returns a status code, not a bool - 0 means success, anything else is
        a specific hardware-reported failure*/
        int engageLock(int zoneCode);
        int releaseLock(int zoneCode);
        bool isEngaged(int zoneCode) const;
};

#endif