#include "Report.h"
#include "Types.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string formatTimePoint(std::chrono::system_clock::time_point timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* localTime = std::localtime(&time);
    if (!localTime) {
        return "Unknown";
    }

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M");
    return oss.str();
}

void printSlot(const ParkingSlot& slot) {
    std::cout << "  Slot ID: " << slot.getId()
              << " | Type: " << vehicleTypeToString(slot.getVehicleType())
              << " | Zone: " << slot.getZone()
              << " | Status: " << slotStatusToString(slot.getStatus()) << "\n";
}

}  // namespace

/**
 * OOP - Polymorphism example:
 * AvailableSlotsReport can be used wherever a Report is expected.
 * Its display() implementation traverses only available slots.
 */
std::string AvailableSlotsReport::getTitle() const {
    return "Available Parking Slots";
}

void AvailableSlotsReport::display(const ParkingSystem& system) const {
    std::cout << "\n--- " << getTitle() << " ---\n";
    auto slots = system.getAvailableSlots();

    if (slots.empty()) {
        std::cout << "No available slots at the moment.\n";
        return;
    }

    for (const auto& slot : slots) {
        printSlot(slot);
    }
}

std::string ParkedVehiclesReport::getTitle() const {
    return "Currently Parked Vehicles";
}

void ParkedVehiclesReport::display(const ParkingSystem& system) const {
    std::cout << "\n--- " << getTitle() << " ---\n";
    auto records = system.getActiveParkingRecords();

    if (records.empty()) {
        std::cout << "No vehicles are currently parked.\n";
        return;
    }

    for (const auto& record : records) {
        std::cout << "  Plate: " << record.getPlateNumber()
                  << " | Type: " << vehicleTypeToString(record.getVehicleType())
                  << " | Slot: " << record.getSlotId()
                  << " | Entry: " << formatTimePoint(record.getEntryTime()) << "\n";
    }
}

std::string TransactionHistoryReport::getTitle() const {
    return "Parking Transaction History";
}

void TransactionHistoryReport::display(const ParkingSystem& system) const {
    std::cout << "\n--- " << getTitle() << " ---\n";
    auto history = system.getTransactionHistory();

    if (history.empty()) {
        std::cout << "No completed transactions yet.\n";
        return;
    }

    for (const auto& tx : history) {
        std::cout << "  Plate: " << tx.getPlateNumber()
                  << " | Slot: " << tx.getSlotId()
                  << " | Exit: " << formatTimePoint(tx.getExitTime())
                  << " | Fee: " << std::fixed << std::setprecision(2) << tx.getTotalFee()
                  << " RWF (Rate: " << tx.getHourlyRate() << ")\n";
    }
}
