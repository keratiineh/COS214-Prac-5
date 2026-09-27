#include "LegacyAccessAdapter.h"

LegacyAccessAdapter::LegacyAccessAdapter(LegacyAccessPanel& panel)
    : panel(panel) {}

int LegacyAccessAdapter::zoneCodeFor(const std::string& area) const {
    auto it = zoneCodes.find(area);
    if (it != zoneCodes.end()) {
        return it->second;
    }
    int code = nextZoneCode++;
    zoneCodes[area] = code;
    return code;
}

bool LegacyAccessAdapter::lock(const std::string& area) {
    int code = zoneCodeFor(area);
    int status = panel.engageLock(code);
    return status == LegacyAccessPanel::STATUS_OK;
}

bool LegacyAccessAdapter::unlock(const std::string& area) {
    int code = zoneCodeFor(area);
    int status = panel.releaseLock(code);
    return status == LegacyAccessPanel::STATUS_OK;
}

bool LegacyAccessAdapter::isLocked(const std::string& area) const {
    int code = zoneCodeFor(area);
    return panel.isEngaged(code);
}