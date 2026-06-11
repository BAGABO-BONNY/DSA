/**
 * ParkingSlot.h
 * -------------
 * Domain model for a single physical parking space.
 *
 * OOP - Encapsulation: all attributes are private; external code uses getters
 * and controlled setters instead of modifying fields directly.
 *
 * Task 1 attributes: Slot ID, vehicle type, zone, status.
 */
#ifndef PARKING_SLOT_H
#define PARKING_SLOT_H

#include "Types.h"
#include <string>

class ParkingSlot {
public:
    ParkingSlot(const std::string& id, VehicleType vehicleType, const std::string& zone);

    const std::string& getId() const;
    VehicleType getVehicleType() const;
    const std::string& getZone() const;
    SlotStatus getStatus() const;

    // UPDATE operation: mark slot Available/Occupied on entry/exit.
    void setStatus(SlotStatus status);

    bool isAvailable() const;
    bool supports(VehicleType vehicleType) const;

private:
    std::string id_;              // Unique slot identifier
    VehicleType vehicleType_;     // Motorcycle / Car / Truck
    std::string zone_;            // Physical location zone
    SlotStatus status_;           // Available or Occupied
};

#endif
