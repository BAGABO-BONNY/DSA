/**
 * ParkingRecord.h
 * ---------------
 * Represents an active (not yet exited) parking session.
 *
 * OOP - Encapsulation: immutable-style read access after creation; the record
 * lives only while the vehicle is still inside the parking area.
 *
 * Task 2 attributes: plate number, vehicle type, entry time, allocated slot.
 */
#ifndef PARKING_RECORD_H
#define PARKING_RECORD_H

#include "Types.h"
#include <chrono>
#include <string>

class ParkingRecord {
public:
    ParkingRecord(const std::string& plateNumber,
                  VehicleType vehicleType,
                  const std::string& slotId,
                  std::chrono::system_clock::time_point entryTime);

    const std::string& getPlateNumber() const;
    VehicleType getVehicleType() const;
    const std::string& getSlotId() const;
    std::chrono::system_clock::time_point getEntryTime() const;

private:
    std::string plateNumber_;
    VehicleType vehicleType_;
    std::string slotId_;
    std::chrono::system_clock::time_point entryTime_;
};

#endif
