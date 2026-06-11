#include "ParkingSlot.h"

// INSERT: new slots always start as Available.
ParkingSlot::ParkingSlot(const std::string& id, VehicleType vehicleType, const std::string& zone)
    : id_(id), vehicleType_(vehicleType), zone_(zone), status_(SlotStatus::Available) {}

const std::string& ParkingSlot::getId() const { return id_; }

VehicleType ParkingSlot::getVehicleType() const { return vehicleType_; }

const std::string& ParkingSlot::getZone() const { return zone_; }

SlotStatus ParkingSlot::getStatus() const { return status_; }

// UPDATE: called when a vehicle enters (Occupied) or exits (Available).
void ParkingSlot::setStatus(SlotStatus status) { status_ = status; }

bool ParkingSlot::isAvailable() const { return status_ == SlotStatus::Available; }

// Allocation rule: slot type must match the arriving vehicle type.
bool ParkingSlot::supports(VehicleType vehicleType) const {
    return vehicleType_ == vehicleType;
}
