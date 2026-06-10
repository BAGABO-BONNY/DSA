/**
 * ParkingSystem.h
 * ---------------
 * Core controller for the Smart Parking Management System.
 *
 * DSA summary:
 *   1. unordered_map<string, ParkingSlot> slotsById_
 *      - Non-linear hash table
 *      - INSERT: configureSlot() via emplace
 *      - UPDATE: setStatus() on entry/exit
 *      - LOOKUP: find slot by ID in O(1) average time
 *      - TRAVERSAL: iterate all slots for reports
 *
 *   2. unordered_map<string, ParkingRecord> activeVehiclesByPlate_
 *      - Non-linear hash table keyed by plate number
 *      - INSERT: registerVehicleEntry() via emplace
 *      - DELETE: processVehicleExit() via erase
 *      - LOOKUP: duplicate-entry prevention in O(1) average time
 *      - TRAVERSAL: list currently parked vehicles
 *
 *   3. vector<ParkingTransaction> transactionHistory_
 *      - Linear dynamic array
 *      - INSERT: push_back() on successful exit
 *      - TRAVERSAL: history, vehicle history, daily revenue
 *      - Justification: append-only completed records; sequential scan
 *        matches chronological reporting needs
 *
 * OOP - Abstraction: exposes high-level operations; hides allocation,
 * billing, and internal data structure details from the UI layer.
 * OOP - Composition: owns PricingManager and domain collections.
 */
#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include "ParkingRecord.h"
#include "ParkingSlot.h"
#include "ParkingTransaction.h"
#include "PricingManager.h"
#include "Types.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

class ParkingSystem {
public:
    ParkingSystem();

    // Task 1: slot configuration (INSERT into slotsById_).
    bool configureSlot(const std::string& slotId,
                       VehicleType vehicleType,
                       const std::string& zone,
                       std::string& errorMessage);

    // Task 2: vehicle entry with automatic slot allocation.
    bool registerVehicleEntry(const std::string& plateNumber,
                              VehicleType vehicleType,
                              std::chrono::system_clock::time_point entryTime,
                              std::string& allocatedSlotId,
                              std::string& errorMessage);

    // Task 2: vehicle entry with preferred slot.
    bool registerVehicleEntry(const std::string& plateNumber,
                              VehicleType vehicleType,
                              std::chrono::system_clock::time_point entryTime,
                              const std::string& preferredSlotId,
                              std::string& allocatedSlotId,
                              std::string& errorMessage);

    // Task 4: vehicle exit, billing, slot release, history insert.
    bool processVehicleExit(const std::string& plateNumber,
                            std::chrono::system_clock::time_point exitTime,
                            ParkingTransaction& transaction,
                            std::string& errorMessage);

    // Task 3: runtime price update (does not alter stored transactions).
    bool updateParkingPrice(VehicleType vehicleType, double newRate, std::string& errorMessage);

    // Report/query operations (TRAVERSAL over maps and vector).
    std::vector<ParkingSlot> getAllSlots() const;
    std::vector<ParkingSlot> getAvailableSlots() const;
    std::vector<ParkingSlot> getAvailableSlotsByType(VehicleType type) const;
    std::vector<ParkingRecord> getActiveParkingRecords() const;
    std::vector<ParkingTransaction> getTransactionHistory() const;
    std::vector<ParkingTransaction> getTransactionsByPlate(const std::string& plateNumber) const;
    double getDailyRevenue(const std::string& date) const;
    bool getDailyRevenue(const std::string& date, double& revenue, std::string& errorMessage) const;

    bool slotExists(const std::string& slotId) const;
    bool isVehicleParked(const std::string& plateNumber) const;
    double getCurrentRate(VehicleType type) const;
    bool hasConfiguredRate(VehicleType type) const;

private:
    // NON-LINEAR: hash map for unique slot lookup and status updates.
    std::unordered_map<std::string, ParkingSlot> slotsById_;

    // NON-LINEAR: hash map for active vehicle tracking and duplicate prevention.
    std::unordered_map<std::string, ParkingRecord> activeVehiclesByPlate_;

    // LINEAR: append-only completed transaction log.
    std::vector<ParkingTransaction> transactionHistory_;

    PricingManager pricingManager_;

    std::vector<std::string> findAvailableSlotIds(VehicleType vehicleType) const;
    int calculateBilledHours(long long durationMinutes) const;
    long long calculateDurationMinutes(std::chrono::system_clock::time_point entry,
                                       std::chrono::system_clock::time_point exit) const;
    std::string formatDate(std::chrono::system_clock::time_point timePoint) const;
};

#endif
