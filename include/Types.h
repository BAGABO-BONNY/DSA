/**
 * Types.h
 * -------
 * Shared domain enumerations and parsing helpers used across the system.
 *
 * OOP - Abstraction: enum class restricts values to a safe, typed set instead
 * of raw strings or magic numbers scattered through the codebase.
 */
#ifndef TYPES_H
#define TYPES_H

#include <string>

// Supported vehicle categories for slots and parking records.
enum class VehicleType { Motorcycle, Car, Truck };

// Slot availability state used during allocation and release.
enum class SlotStatus { Available, Occupied };

// Convert enums to readable text for console output (traversal/display).
std::string vehicleTypeToString(VehicleType type);
std::string slotStatusToString(SlotStatus status);

// Parse and validate user/menu input into typed enums.
bool parseVehicleType(const std::string& input, VehicleType& out);
bool parseSlotStatus(const std::string& input, SlotStatus& out);

#endif
