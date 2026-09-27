#include "LegacyAccessPanel.h"

int LegacyAccessPanel::engageLock(int zoneCode) {
    if (zoneCode < 0 || zoneCode >= 256) return STATUS_UNKNOWN_ZONE;
    if (engagedZones[zoneCode]) return STATUS_ALREADY_ENGAGED;
    engagedZones[zoneCode] = true;
    return STATUS_OK;
}

int LegacyAccessPanel::releaseLock(int zoneCode) {
    if (zoneCode < 0 || zoneCode >= 256) return STATUS_UNKNOWN_ZONE;
    if (!engagedZones[zoneCode]) return STATUS_ALREADY_RELEASED;
    engagedZones[zoneCode] = false;
    return STATUS_OK;
}

bool LegacyAccessPanel::isEngaged(int zoneCode) const {
    if (zoneCode < 0 || zoneCode >= 256) return false;
    return engagedZones[zoneCode];
}