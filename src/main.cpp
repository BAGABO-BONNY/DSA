/**
 * main.cpp
 * --------
 * Console-based, menu-driven user interface for the Smart Parking System.
 *
 * This file demonstrates:
 *   - Menu-driven interaction (requirement)
 *   - Input validation before calling business logic
 *   - Polymorphic report usage through Report subclasses
 *   - Built-in demo test scenario (option 12) with sample inputs
 *
 * Suggested manual test inputs:
 *   Slot IDs : M-01, C-01, T-01
 *   Plates   : RAA 123A, RAE 710F, RBG 456K
 *   Times    : HH:MM or YYYY-MM-DD HH:MM
 */
#include "InputValidator.h"
#include "ParkingSystem.h"
#include "Report.h"
#include "Types.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
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

std::string currentDateString() {
    auto now = std::chrono::system_clock::now();
    return formatTimePoint(now).substr(0, 10);
}

void pause() {
    std::cout << "\nPress Enter to continue...";

    if (!std::cin.good()) {
        std::cin.clear();
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (std::cin.eof()) {
        std::cin.clear();
    }
}

void printHeader() {
    std::cout << "\n========================================\n";
    std::cout << " KIGALI SMART PARKING MANAGEMENT SYSTEM\n";
    std::cout << "========================================\n";
}

// Menu-driven interface required by the project brief.
void printMainMenu() {
    printHeader();
    std::cout << "1.  Configure Parking Slot\n";
    std::cout << "2.  View All Parking Slots\n";
    std::cout << "3.  View Available Slots\n";
    std::cout << "4.  Register Vehicle Entry\n";
    std::cout << "5.  Process Vehicle Exit\n";
    std::cout << "6.  View Currently Parked Vehicles\n";
    std::cout << "7.  View Parking Transaction History\n";
    std::cout << "8.  View Vehicle History by Plate\n";
    std::cout << "9.  View Daily Revenue Report\n";
    std::cout << "10. Update Parking Prices\n";
    std::cout << "11. View Current Parking Rates\n";
    std::cout << "12. Run Demo Test Scenario\n";
    std::cout << "0.  Exit\n";
}

VehicleType readVehicleType() {
    while (true) {
        std::string input = InputValidator::readLine(
            "Vehicle type (Motorcycle/Car/Truck or 1/2/3): ");

        if (!InputValidator::isNonEmpty(input)) {
            std::cout << "Vehicle type cannot be empty. Try again.\n";
            continue;
        }

        VehicleType type;
        if (parseVehicleType(input, type)) {
            return type;
        }

        std::cout << "Invalid vehicle type. Enter Motorcycle, Car, Truck, or 1/2/3.\n";
    }
}

std::chrono::system_clock::time_point readDateTime(const std::string& label,
                                                  bool checkPastLimit = true) {
    while (true) {
        std::string input = InputValidator::readLine(label + " (YYYY-MM-DD HH:MM or HH:MM): ");
        std::chrono::system_clock::time_point parsed;
        std::string error;

        if (!InputValidator::parseDateTime(input, parsed, error)) {
            std::cout << "Error: " << error << "\n";
            continue;
        }

        if (!InputValidator::isNotFutureTime(parsed, error)) {
            std::cout << "Error: " << error << "\n";
            continue;
        }

        if (checkPastLimit && !InputValidator::isReasonablePastTime(parsed, error)) {
            std::cout << "Error: " << error << "\n";
            continue;
        }

        return parsed;
    }
}

void printSlot(const ParkingSlot& slot) {
    std::cout << "  Slot ID: " << slot.getId()
              << " | Type: " << vehicleTypeToString(slot.getVehicleType())
              << " | Zone: " << slot.getZone()
              << " | Status: " << slotStatusToString(slot.getStatus()) << "\n";
}

// Task 1 handler: configure a new slot.
void handleConfigureSlot(ParkingSystem& system) {
    std::cout << "\n--- Configure Parking Slot ---\n";

    std::string slotId = InputValidator::readNonEmptyLine("Slot ID: ");
    std::string error;

    if (!InputValidator::isValidSlotId(slotId, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    if (system.slotExists(slotId)) {
        std::cout << "Error: Slot ID already exists.\n";
        return;
    }

    VehicleType type = readVehicleType();
    std::string zone = InputValidator::readNonEmptyLine("Zone: ");

    if (!InputValidator::isValidZone(zone, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    if (system.configureSlot(slotId, type, zone, error)) {
        std::cout << "Success: Slot '" << slotId << "' configured as "
                  << vehicleTypeToString(type) << " in zone '" << zone << "'.\n";
    } else {
        std::cout << "Error: " << error << "\n";
    }
}

void handleViewAllSlots(const ParkingSystem& system) {
    std::cout << "\n--- All Parking Slots ---\n";
    auto slots = system.getAllSlots();

    if (slots.empty()) {
        std::cout << "No parking slots configured.\n";
        return;
    }

    for (const auto& slot : slots) {
        printSlot(slot);
    }
}

// Polymorphism: concrete report object used through Report interface behavior.
void handleViewAvailableSlots(const ParkingSystem& system) {
    AvailableSlotsReport report;
    report.display(system);
}

// Task 2 handler: register vehicle entry.
void handleVehicleEntry(ParkingSystem& system) {
    std::cout << "\n--- Register Vehicle Entry ---\n";

    if (system.getAllSlots().empty()) {
        std::cout << "Error: No parking slots configured. Please configure slots first (menu 1).\n";
        return;
    }

    std::string plate = InputValidator::readNonEmptyLine("Plate number: ");
    std::string error;

    if (!InputValidator::isValidPlateNumber(plate, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    if (system.isVehicleParked(plate)) {
        std::cout << "Error: Vehicle is already parked.\n";
        return;
    }

    VehicleType type = readVehicleType();

    auto available = system.getAvailableSlotsByType(type);
    if (available.empty()) {
        std::cout << "Error: No suitable parking slot is currently available for "
                  << vehicleTypeToString(type) << ".\n";
        return;
    }

    std::cout << "Available slots for " << vehicleTypeToString(type) << ":\n";
    for (const auto& slot : available) {
        printSlot(slot);
    }

    std::string preferred = InputValidator::readLine(
        "Preferred slot ID (leave blank for automatic allocation): ");

    if (!preferred.empty() && !InputValidator::isValidSlotId(preferred, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    auto entryTime = readDateTime("Entry time");

    std::string allocatedSlot;
    bool success = false;

    if (preferred.empty()) {
        success = system.registerVehicleEntry(plate, type, entryTime, allocatedSlot, error);
    } else {
        success = system.registerVehicleEntry(plate, type, entryTime, preferred, allocatedSlot, error);
    }

    if (success) {
        std::cout << "Success: Vehicle " << plate << " parked in slot "
                  << allocatedSlot << " at " << formatTimePoint(entryTime) << ".\n";
    } else {
        std::cout << "Error: " << error << "\n";
    }
}

// Task 4 handler: process exit, show receipt, store history.
void handleVehicleExit(ParkingSystem& system) {
    std::cout << "\n--- Process Vehicle Exit ---\n";

    std::string plate = InputValidator::readNonEmptyLine("Plate number: ");
    std::string error;

    if (!InputValidator::isValidPlateNumber(plate, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    if (!system.isVehicleParked(plate)) {
        std::cout << "Error: Vehicle is not currently parked.\n";
        return;
    }

    auto exitTime = readDateTime("Exit time", false);
    ParkingTransaction transaction("", VehicleType::Car, "", "", std::chrono::system_clock::now(),
                                   std::chrono::system_clock::now(), 0, 0, 0.0, 0.0);

    if (system.processVehicleExit(plate, exitTime, transaction, error)) {
        std::cout << "\n--- Parking Receipt ---\n";
        std::cout << "Plate Number   : " << transaction.getPlateNumber() << "\n";
        std::cout << "Vehicle Type   : " << vehicleTypeToString(transaction.getVehicleType()) << "\n";
        std::cout << "Slot ID        : " << transaction.getSlotId() << "\n";
        std::cout << "Zone           : " << transaction.getZone() << "\n";
        std::cout << "Entry Time     : " << formatTimePoint(transaction.getEntryTime()) << "\n";
        std::cout << "Exit Time      : " << formatTimePoint(transaction.getExitTime()) << "\n";
        std::cout << "Duration       : " << transaction.getDurationMinutes() << " minute(s)\n";
        std::cout << "Billed Hours   : " << transaction.getBilledHours() << " hour(s)\n";
        std::cout << "Hourly Rate    : " << std::fixed << std::setprecision(2)
                  << transaction.getHourlyRate() << " RWF\n";
        std::cout << "Total Fee      : " << std::fixed << std::setprecision(2)
                  << transaction.getTotalFee() << " RWF\n";
        std::cout << "Slot released and transaction stored in history.\n";
    } else {
        std::cout << "Error: " << error << "\n";
    }
}

void handleParkedVehicles(const ParkingSystem& system) {
    ParkedVehiclesReport report;
    report.display(system);
}

void handleTransactionHistory(const ParkingSystem& system) {
    TransactionHistoryReport report;
    report.display(system);
}

void handleVehicleHistory(const ParkingSystem& system) {
    std::cout << "\n--- Vehicle History by Plate ---\n";
    std::string plate = InputValidator::readNonEmptyLine("Plate number: ");
    std::string error;

    if (!InputValidator::isValidPlateNumber(plate, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    auto history = system.getTransactionsByPlate(plate);
    if (history.empty()) {
        std::cout << "No transaction history found for plate " << plate << ".\n";
        return;
    }

    for (const auto& tx : history) {
        std::cout << "  Entry: " << formatTimePoint(tx.getEntryTime())
                  << " | Exit: " << formatTimePoint(tx.getExitTime())
                  << " | Billed Hours: " << tx.getBilledHours()
                  << " | Fee: " << std::fixed << std::setprecision(2) << tx.getTotalFee()
                  << " RWF\n";
    }
}

void handleDailyRevenue(const ParkingSystem& system) {
    std::cout << "\n--- Daily Revenue Report ---\n";
    std::string date = InputValidator::readLine(
        "Date (YYYY-MM-DD, blank for today): ");

    if (date.empty()) {
        date = currentDateString();
    }

    std::string error;
    double revenue = 0.0;
    if (!system.getDailyRevenue(date, revenue, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    std::cout << "Total revenue for " << date << ": "
              << std::fixed << std::setprecision(2) << revenue << " RWF\n";
}

// Task 3 handler: update active prices without changing history.
void handleUpdatePrices(ParkingSystem& system) {
    std::cout << "\n--- Update Parking Prices ---\n";
    std::cout << "Note: Price updates affect only future exits.\n";
    std::cout << "Completed transactions keep their original rates.\n";

    VehicleType type = readVehicleType();

    double newRate = 0.0;
    InputValidator::readPositiveRate(newRate, "New hourly rate (RWF): ");

    std::string error;
    if (system.updateParkingPrice(type, newRate, error)) {
        std::cout << "Success: " << vehicleTypeToString(type)
                  << " rate updated to " << std::fixed << std::setprecision(2)
                  << newRate << " RWF/hour.\n";
    } else {
        std::cout << "Error: " << error << "\n";
    }
}

void handleViewRates(const ParkingSystem& system) {
    std::cout << "\n--- Current Parking Rates ---\n";
    std::cout << "Motorcycle: " << system.getCurrentRate(VehicleType::Motorcycle) << " RWF/hour\n";
    std::cout << "Car       : " << system.getCurrentRate(VehicleType::Car) << " RWF/hour\n";
    if (system.hasConfiguredRate(VehicleType::Truck)) {
        std::cout << "Truck     : " << system.getCurrentRate(VehicleType::Truck) << " RWF/hour\n";
    } else {
        std::cout << "Truck     : Not configured (use menu option 10 to set a rate)\n";
    }
}

/**
 * Built-in automated test scenario (menu option 12).
 * Demonstrates all major functionalities without manual typing:
 *   - slot configuration
 *   - successful entries
 *   - duplicate entry rejection
 *   - no available slot rejection
 *   - exit billing with ceiling hours
 *   - price update
 *   - daily revenue
 */
void runDemoScenario(ParkingSystem& system) {
    std::cout << "\n--- Running Demo Test Scenario ---\n";

    std::string error;
    std::string allocated;

    system.configureSlot("M-01", VehicleType::Motorcycle, "Zone A", error);
    system.configureSlot("C-01", VehicleType::Car, "Zone A", error);
    system.configureSlot("C-02", VehicleType::Car, "Zone B", error);
    system.configureSlot("T-01", VehicleType::Truck, "Zone C", error);

    auto baseTime = std::chrono::system_clock::now() - std::chrono::hours(3);

    system.registerVehicleEntry("RAA 123A", VehicleType::Car, baseTime, allocated, error);
    system.registerVehicleEntry("RAE 710F", VehicleType::Motorcycle, baseTime + std::chrono::minutes(10),
                                allocated, error);

    std::string duplicateError;
    system.registerVehicleEntry("RAA 123A", VehicleType::Car, baseTime, allocated, duplicateError);

    auto truckTime = baseTime + std::chrono::minutes(20);
    system.registerVehicleEntry("RBG 456K", VehicleType::Truck, truckTime, allocated, error);

    std::string noSlotError;
    system.registerVehicleEntry("RBT 888X", VehicleType::Truck, truckTime + std::chrono::minutes(5),
                                allocated, noSlotError);

    ParkingTransaction firstExit("", VehicleType::Car, "", "", baseTime, baseTime, 0, 0, 0.0, 0.0);
    system.processVehicleExit("RAE 710F", baseTime + std::chrono::minutes(25), firstExit, error);

    system.updateParkingPrice(VehicleType::Car, 1200.0, error);

    ParkingTransaction secondExit("", VehicleType::Car, "", "", baseTime, baseTime, 0, 0, 0.0, 0.0);
    system.processVehicleExit("RAA 123A", baseTime + std::chrono::hours(1) + std::chrono::minutes(20),
                              secondExit, error);

    std::cout << "Demo configured 4 slots.\n";
    std::cout << "Duplicate entry blocked: " << duplicateError << "\n";
    std::cout << "No-slot entry blocked : " << noSlotError << "\n";
    std::cout << "Motorcycle exit fee   : " << std::fixed << std::setprecision(2)
              << firstExit.getTotalFee() << " RWF ("
              << firstExit.getBilledHours() << " billed hour(s))\n";
    std::cout << "Car exit fee          : " << std::fixed << std::setprecision(2)
              << secondExit.getTotalFee() << " RWF ("
              << secondExit.getBilledHours() << " billed hour(s))\n";
    std::cout << "Today's revenue       : " << system.getDailyRevenue(currentDateString())
              << " RWF\n";
    std::cout << "Use menu options 2, 6, 7, and 9 to inspect demo results.\n";
}

}  // namespace

int main() {
    ParkingSystem system;

    while (true) {
        printMainMenu();
        int choice = InputValidator::readMenuChoice(0, 12);

        switch (choice) {
            case 1:
                handleConfigureSlot(system);
                pause();
                break;
            case 2:
                handleViewAllSlots(system);
                pause();
                break;
            case 3:
                handleViewAvailableSlots(system);
                pause();
                break;
            case 4:
                handleVehicleEntry(system);
                pause();
                break;
            case 5:
                handleVehicleExit(system);
                pause();
                break;
            case 6:
                handleParkedVehicles(system);
                pause();
                break;
            case 7:
                handleTransactionHistory(system);
                pause();
                break;
            case 8:
                handleVehicleHistory(system);
                pause();
                break;
            case 9:
                handleDailyRevenue(system);
                pause();
                break;
            case 10:
                handleUpdatePrices(system);
                pause();
                break;
            case 11:
                handleViewRates(system);
                pause();
                break;
            case 12:
                runDemoScenario(system);
                pause();
                break;
            case 0:
                std::cout << "\nExiting Kigali Smart Parking System. Goodbye!\n";
                return 0;
            default:
                std::cout << "Invalid choice.\n";
                pause();
                break;
        }
    }
}
