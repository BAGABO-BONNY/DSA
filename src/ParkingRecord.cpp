#include "ParkingRecord.h"

// INSERT: created when a vehicle successfully enters the parking area.
ParkingRecord::ParkingRecord(const std::string& plateNumber,
                           VehicleType vehicleType,
                           const std::string& slotId,
                           std::chrono::system_clock::time_point entryTime)
    : plateNumber_(plateNumber),
      vehicleType_(vehicleType),
      slotId_(slotId),
      entryTime_(entryTime) {}

const std::string& ParkingRecord::getPlateNumber() const { return plateNumber_; }

VehicleType ParkingRecord::getVehicleType() const { return vehicleType_; }

const std::string& ParkingRecord::getSlotId() const { return slotId_; }

std::chrono::system_clock::time_point ParkingRecord::getEntryTime() const { return entryTime_; }
