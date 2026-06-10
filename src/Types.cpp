#include "Types.h"

#include <algorithm>
#include <cctype>

// Display helper for reports and receipts.
std::string vehicleTypeToString(VehicleType type) {
    switch (type) {
        case VehicleType::Motorcycle:
            return "Motorcycle";
        case VehicleType::Car:
            return "Car";
        case VehicleType::Truck:
            return "Truck";
    }
    return "Unknown";
}

std::string slotStatusToString(SlotStatus status) {
    switch (status) {
        case SlotStatus::Available:
            return "Available";
        case SlotStatus::Occupied:
            return "Occupied";
    }
    return "Unknown";
}

// Validation: accept names or numeric shortcuts from the menu.
bool parseVehicleType(const std::string& input, VehicleType& out) {
    std::string normalized = input;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (normalized == "motorcycle" || normalized == "1") {
        out = VehicleType::Motorcycle;
        return true;
    }
    if (normalized == "car" || normalized == "2") {
        out = VehicleType::Car;
        return true;
    }
    if (normalized == "truck" || normalized == "3") {
        out = VehicleType::Truck;
        return true;
    }
    return false;
}

bool parseSlotStatus(const std::string& input, SlotStatus& out) {
    std::string normalized = input;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (normalized == "available" || normalized == "1") {
        out = SlotStatus::Available;
        return true;
    }
    if (normalized == "occupied" || normalized == "2") {
        out = SlotStatus::Occupied;
        return true;
    }
    return false;
}
