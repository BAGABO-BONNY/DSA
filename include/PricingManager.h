/**
 * PricingManager.h
 * ----------------
 * Manages active hourly parking tariffs.
 *
 * DSA - Non-linear structure: unordered_map<VehicleType, double>
 *   - INSERT: set default or updated rate
 *   - UPDATE: overwrite rate for a vehicle type at runtime
 *   - LOOKUP: O(1) average retrieval by vehicle type at exit
 *
 * Justification: only three vehicle types exist, but map lookup is clean,
 * extensible, and avoids long switch/case blocks in billing logic.
 *
 * OOP - Separation of concerns: pricing is isolated from slot allocation.
 */
#ifndef PRICING_MANAGER_H
#define PRICING_MANAGER_H

#include "Types.h"
#include <unordered_map>

class PricingManager {
public:
    PricingManager();

    // TRAVERSAL/LOOKUP: read current active rate.
    double getHourlyRate(VehicleType type) const;
    bool hasHourlyRate(VehicleType type) const;

    // UPDATE: change rate for future exits only.
    bool updateHourlyRate(VehicleType type, double newRate, std::string& errorMessage);

    std::unordered_map<VehicleType, double> getAllRates() const;
    void replaceAllRates(const std::unordered_map<VehicleType, double>& rates);

private:
    // Non-linear hash map for fast tariff lookup and update.
    std::unordered_map<VehicleType, double> hourlyRates_;
};

#endif
