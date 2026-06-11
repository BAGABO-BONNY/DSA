#include "ParkingSystem.h"

#include "InputValidator.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

namespace {

constexpr const char* DATA_VERSION = "PARKING_DATA_V1";

std::time_t toTimeT(std::chrono::system_clock::time_point timePoint) {
    return std::chrono::system_clock::to_time_t(timePoint);
}

std::chrono::system_clock::time_point fromTimeT(std::time_t time) {
    return std::chrono::system_clock::from_time_t(time);
}

std::vector<std::string> splitFields(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);

    while (std::getline(stream, field, delimiter)) {
        fields.push_back(field);
    }

    return fields;
}

bool parseStoredVehicleType(const std::string& value, VehicleType& out) {
    if (value == "Motorcycle") {
        out = VehicleType::Motorcycle;
        return true;
    }
    if (value == "Car") {
        out = VehicleType::Car;
        return true;
    }
    if (value == "Truck") {
        out = VehicleType::Truck;
        return true;
    }
    return false;
}

bool parseStoredSlotStatus(const std::string& value, SlotStatus& out) {
    if (value == "Available") {
        out = SlotStatus::Available;
        return true;
    }
    if (value == "Occupied") {
        out = SlotStatus::Occupied;
        return true;
    }
    return false;
}

}  // namespace

ParkingSystem::ParkingSystem() : dataFilePath_(DEFAULT_DATA_FILE) {
    loadPersistedState();
}

/**
 * Task 1 - Parking Slot Configuration
 * DSA operation: INSERT into unordered_map slotsById_
 * Time complexity: O(1) average
 */
bool ParkingSystem::configureSlot(const std::string& slotId,
                                  VehicleType vehicleType,
                                  const std::string& zone,
                                  std::string& errorMessage) {
    if (!InputValidator::isValidSlotId(slotId, errorMessage)) {
        return false;
    }

    if (!InputValidator::isValidZone(zone, errorMessage)) {
        return false;
    }

    if (slotsById_.size() >= static_cast<size_t>(InputValidator::MAX_SLOTS_ALLOWED)) {
        errorMessage = "Maximum number of parking slots (" +
                       std::to_string(InputValidator::MAX_SLOTS_ALLOWED) + ") has been reached.";
        return false;
    }

    const std::string normalizedSlotId = InputValidator::normalizeIdentifier(slotId);

    // Validation: slot IDs must remain unique (case-insensitive).
    if (slotsById_.find(normalizedSlotId) != slotsById_.end()) {
        errorMessage = "Slot ID already exists. Slot IDs must be unique.";
        return false;
    }

    // INSERT new slot object into hash map keyed by normalized slot ID.
    slotsById_.emplace(normalizedSlotId, ParkingSlot(normalizedSlotId, vehicleType, zone));

    if (!persistState()) {
        std::cout << "Warning: slot configured but data could not be saved to disk.\n";
    }

    return true;
}

bool ParkingSystem::registerVehicleEntry(const std::string& plateNumber,
                                           VehicleType vehicleType,
                                           std::chrono::system_clock::time_point entryTime,
                                           std::string& allocatedSlotId,
                                           std::string& errorMessage) {
    return registerVehicleEntry(plateNumber, vehicleType, entryTime, "", allocatedSlotId, errorMessage);
}

/**
 * Task 2 - Vehicle Entry Management
 * DSA operations:
 *   - LOOKUP slot availability in unordered_map slotsById_
 *   - UPDATE slot status to Occupied
 *   - INSERT active record into unordered_map activeVehiclesByPlate_
 */
bool ParkingSystem::registerVehicleEntry(const std::string& plateNumber,
                                           VehicleType vehicleType,
                                           std::chrono::system_clock::time_point entryTime,
                                           const std::string& preferredSlotId,
                                           std::string& allocatedSlotId,
                                           std::string& errorMessage) {
    if (!InputValidator::isValidPlateNumber(plateNumber, errorMessage)) {
        return false;
    }

    const std::string normalizedPlate = InputValidator::normalizeIdentifier(plateNumber);

    // Validation: one active parking session per plate number.
    if (activeVehiclesByPlate_.find(normalizedPlate) != activeVehiclesByPlate_.end()) {
        errorMessage = "Vehicle is already parked. A vehicle cannot be parked more than once at the same time.";
        return false;
    }

    if (!InputValidator::isNotFutureTime(entryTime, errorMessage)) {
        return false;
    }

    if (!InputValidator::isReasonablePastTime(entryTime, errorMessage)) {
        return false;
    }

    if (slotsById_.empty()) {
        errorMessage = "No parking slots configured. Please configure slots first.";
        return false;
    }

    std::string selectedSlotId;

    if (!preferredSlotId.empty()) {
        if (!InputValidator::isValidSlotId(preferredSlotId, errorMessage)) {
            errorMessage = "Preferred slot ID is invalid: " + errorMessage;
            return false;
        }

        const std::string normalizedPreferredSlot = InputValidator::normalizeIdentifier(preferredSlotId);
        auto slotIt = slotsById_.find(normalizedPreferredSlot);
        if (slotIt == slotsById_.end()) {
            errorMessage = "Preferred slot does not exist.";
            return false;
        }

        if (!slotIt->second.isAvailable()) {
            errorMessage = "Preferred slot is not available.";
            return false;
        }

        if (!slotIt->second.supports(vehicleType)) {
            errorMessage = "Preferred slot does not support this vehicle type.";
            return false;
        }

        selectedSlotId = normalizedPreferredSlot;
    } else {
        // TRAVERSAL + FILTER: collect all suitable available slots.
        std::vector<std::string> availableSlots = findAvailableSlotIds(vehicleType);
        if (availableSlots.empty()) {
            errorMessage = "No suitable parking slot is currently available for this vehicle type.";
            return false;
        }

        selectedSlotId = availableSlots.front();
    }

    // UPDATE slot status and INSERT active parking record.
    auto& selectedSlot = slotsById_.at(selectedSlotId);
    selectedSlot.setStatus(SlotStatus::Occupied);

    activeVehiclesByPlate_.emplace(
        normalizedPlate,
        ParkingRecord(normalizedPlate, vehicleType, selectedSlotId, entryTime));

    allocatedSlotId = selectedSlotId;

    if (!persistState()) {
        std::cout << "Warning: vehicle entry recorded but data could not be saved to disk.\n";
    }

    return true;
}

/**
 * Task 4 - Vehicle Exit and Parking Update
 * DSA operations:
 *   - LOOKUP active record in activeVehiclesByPlate_
 *   - UPDATE slot status back to Available
 *   - INSERT completed transaction into vector transactionHistory_
 *   - DELETE active record from activeVehiclesByPlate_
 */
bool ParkingSystem::processVehicleExit(const std::string& plateNumber,
                                         std::chrono::system_clock::time_point exitTime,
                                         ParkingTransaction& transaction,
                                         std::string& errorMessage) {
    if (!InputValidator::isValidPlateNumber(plateNumber, errorMessage)) {
        return false;
    }

    const std::string normalizedPlate = InputValidator::normalizeIdentifier(plateNumber);

    auto activeIt = activeVehiclesByPlate_.find(normalizedPlate);
    if (activeIt == activeVehiclesByPlate_.end()) {
        errorMessage = "Vehicle is not currently parked in the system.";
        return false;
    }

    const ParkingRecord& record = activeIt->second;

    if (!InputValidator::isExitAfterEntry(record.getEntryTime(), exitTime, errorMessage)) {
        return false;
    }

    if (!InputValidator::isNotFutureTime(exitTime, errorMessage)) {
        return false;
    }

    if (!InputValidator::isReasonableParkingDuration(record.getEntryTime(), exitTime, errorMessage)) {
        return false;
    }

    auto slotIt = slotsById_.find(record.getSlotId());
    if (slotIt == slotsById_.end()) {
        errorMessage = "Allocated slot record is missing. System state is inconsistent.";
        return false;
    }

    // Task 3: billing only happens at exit using the current active rate.
    if (!pricingManager_.hasHourlyRate(record.getVehicleType())) {
        errorMessage = "No parking rate configured for this vehicle type. "
                       "Use menu option 10 to set a rate before exit.";
        return false;
    }

    long long durationMinutes = calculateDurationMinutes(record.getEntryTime(), exitTime);
    int billedHours = calculateBilledHours(durationMinutes);
    double hourlyRate = pricingManager_.getHourlyRate(record.getVehicleType());
    double totalFee = billedHours * hourlyRate;

    if (!InputValidator::isFiniteNumber(totalFee)) {
        errorMessage = "Calculated parking fee is invalid. Please contact the system administrator.";
        return false;
    }

    ParkingTransaction completed(
        record.getPlateNumber(),
        record.getVehicleType(),
        record.getSlotId(),
        slotIt->second.getZone(),
        record.getEntryTime(),
        exitTime,
        durationMinutes,
        billedHours,
        hourlyRate,
        totalFee);

    slotIt->second.setStatus(SlotStatus::Available);
    transactionHistory_.push_back(completed);      // LINEAR INSERT
    activeVehiclesByPlate_.erase(activeIt);        // DELETE from active map

    transaction = completed;

    if (!persistState()) {
        std::cout << "Warning: vehicle exit recorded but data could not be saved to disk.\n";
    }

    return true;
}

// Task 3 - controlled runtime price update.
bool ParkingSystem::updateParkingPrice(VehicleType vehicleType, double newRate, std::string& errorMessage) {
    if (!InputValidator::isPositiveRate(newRate, errorMessage)) {
        return false;
    }

    if (!pricingManager_.updateHourlyRate(vehicleType, newRate, errorMessage)) {
        return false;
    }

    if (!persistState()) {
        std::cout << "Warning: price updated but data could not be saved to disk.\n";
    }

    return true;
}

// TRAVERSAL: copy all slots from hash map into sorted vector for display.
std::vector<ParkingSlot> ParkingSystem::getAllSlots() const {
    std::vector<ParkingSlot> slots;
    slots.reserve(slotsById_.size());

    for (const auto& pair : slotsById_) {
        slots.push_back(pair.second);
    }

    std::sort(slots.begin(), slots.end(),
              [](const ParkingSlot& a, const ParkingSlot& b) { return a.getId() < b.getId(); });

    return slots;
}

// TRAVERSAL: filter only available slots.
std::vector<ParkingSlot> ParkingSystem::getAvailableSlots() const {
    std::vector<ParkingSlot> slots;

    for (const auto& pair : slotsById_) {
        if (pair.second.isAvailable()) {
            slots.push_back(pair.second);
        }
    }

    std::sort(slots.begin(), slots.end(),
              [](const ParkingSlot& a, const ParkingSlot& b) { return a.getId() < b.getId(); });

    return slots;
}

std::vector<ParkingSlot> ParkingSystem::getAvailableSlotsByType(VehicleType type) const {
    std::vector<ParkingSlot> slots;

    for (const auto& pair : slotsById_) {
        if (pair.second.isAvailable() && pair.second.supports(type)) {
            slots.push_back(pair.second);
        }
    }

    std::sort(slots.begin(), slots.end(),
              [](const ParkingSlot& a, const ParkingSlot& b) { return a.getId() < b.getId(); });

    return slots;
}

// TRAVERSAL: list all active parking records.
std::vector<ParkingRecord> ParkingSystem::getActiveParkingRecords() const {
    std::vector<ParkingRecord> records;
    records.reserve(activeVehiclesByPlate_.size());

    for (const auto& pair : activeVehiclesByPlate_) {
        records.push_back(pair.second);
    }

    std::sort(records.begin(), records.end(),
              [](const ParkingRecord& a, const ParkingRecord& b) {
                  return a.getPlateNumber() < b.getPlateNumber();
              });

    return records;
}

// TRAVERSAL: return full completed transaction vector.
std::vector<ParkingTransaction> ParkingSystem::getTransactionHistory() const {
    return transactionHistory_;
}

// TRAVERSAL: linear scan of transactionHistory_ filtered by plate.
std::vector<ParkingTransaction> ParkingSystem::getTransactionsByPlate(const std::string& plateNumber) const {
    const std::string normalizedPlate = InputValidator::normalizeIdentifier(plateNumber);

    std::vector<ParkingTransaction> results;

    for (const auto& transaction : transactionHistory_) {
        if (transaction.getPlateNumber() == normalizedPlate) {
            results.push_back(transaction);
        }
    }

    return results;
}

bool ParkingSystem::getDailyRevenue(const std::string& date, double& revenue, std::string& errorMessage) const {
    if (!InputValidator::isNotFutureDate(date, errorMessage)) {
        revenue = 0.0;
        return false;
    }

    double total = 0.0;

    for (const auto& transaction : transactionHistory_) {
        if (formatDate(transaction.getExitTime()) == date) {
            total += transaction.getTotalFee();
            if (!InputValidator::isFiniteNumber(total)) {
                errorMessage = "Daily revenue calculation overflowed. Data may be inconsistent.";
                revenue = 0.0;
                return false;
            }
        }
    }

    revenue = total;
    return true;
}

// TRAVERSAL: aggregate revenue by scanning completed transactions.
double ParkingSystem::getDailyRevenue(const std::string& date) const {
    double revenue = 0.0;
    std::string error;
    getDailyRevenue(date, revenue, error);
    return revenue;
}

bool ParkingSystem::slotExists(const std::string& slotId) const {
    return slotsById_.find(InputValidator::normalizeIdentifier(slotId)) != slotsById_.end();
}

bool ParkingSystem::isVehicleParked(const std::string& plateNumber) const {
    return activeVehiclesByPlate_.find(InputValidator::normalizeIdentifier(plateNumber)) !=
           activeVehiclesByPlate_.end();
}

double ParkingSystem::getCurrentRate(VehicleType type) const {
    return pricingManager_.getHourlyRate(type);
}

bool ParkingSystem::hasConfiguredRate(VehicleType type) const {
    return pricingManager_.hasHourlyRate(type);
}

// Helper TRAVERSAL: collect IDs of all free slots matching a vehicle type.
std::vector<std::string> ParkingSystem::findAvailableSlotIds(VehicleType vehicleType) const {
    std::vector<std::string> available;

    for (const auto& pair : slotsById_) {
        if (pair.second.isAvailable() && pair.second.supports(vehicleType)) {
            available.push_back(pair.first);
        }
    }

    std::sort(available.begin(), available.end());
    return available;
}

/**
 * Task 3 billing rule:
 * Partial hours are charged as full hours.
 * Examples: 15 minutes -> 1 hour, 1 hour 20 minutes -> 2 hours.
 */
int ParkingSystem::calculateBilledHours(long long durationMinutes) const {
    if (durationMinutes <= 0) {
        return 1;
    }

    return static_cast<int>(std::ceil(durationMinutes / 60.0));
}

long long ParkingSystem::calculateDurationMinutes(std::chrono::system_clock::time_point entry,
                                                    std::chrono::system_clock::time_point exit) const {
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(exit - entry);
    return duration.count();
}

std::string ParkingSystem::formatDate(std::chrono::system_clock::time_point timePoint) const {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* localTime = std::localtime(&time);

    if (!localTime) {
        return "";
    }

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d");
    return oss.str();
}

bool ParkingSystem::saveState(std::string& errorMessage) const {
    const std::string tempPath = dataFilePath_ + ".tmp";

    std::ofstream out(tempPath, std::ios::trunc);
    if (!out) {
        errorMessage = "Unable to open data file for writing.";
        return false;
    }

    out << DATA_VERSION << "\n";
    out << "[RATES]\n";

    for (const auto& rateEntry : pricingManager_.getAllRates()) {
        out << vehicleTypeToString(rateEntry.first) << "|"
            << std::fixed << std::setprecision(2) << rateEntry.second << "\n";
    }

    out << "[SLOTS]\n";
    for (const auto& slotEntry : slotsById_) {
        const ParkingSlot& slot = slotEntry.second;
        out << slot.getId() << "|"
            << vehicleTypeToString(slot.getVehicleType()) << "|"
            << slot.getZone() << "|"
            << slotStatusToString(slot.getStatus()) << "\n";
    }

    out << "[ACTIVE]\n";
    for (const auto& activeEntry : activeVehiclesByPlate_) {
        const ParkingRecord& record = activeEntry.second;
        out << record.getPlateNumber() << "|"
            << vehicleTypeToString(record.getVehicleType()) << "|"
            << record.getSlotId() << "|"
            << toTimeT(record.getEntryTime()) << "\n";
    }

    out << "[TRANSACTIONS]\n";
    for (const auto& transaction : transactionHistory_) {
        out << transaction.getPlateNumber() << "|"
            << vehicleTypeToString(transaction.getVehicleType()) << "|"
            << transaction.getSlotId() << "|"
            << transaction.getZone() << "|"
            << toTimeT(transaction.getEntryTime()) << "|"
            << toTimeT(transaction.getExitTime()) << "|"
            << transaction.getDurationMinutes() << "|"
            << transaction.getBilledHours() << "|"
            << std::fixed << std::setprecision(2) << transaction.getHourlyRate() << "|"
            << std::fixed << std::setprecision(2) << transaction.getTotalFee() << "\n";
    }

    out << "[END]\n";

    if (!out.good()) {
        errorMessage = "Failed while writing parking data to disk.";
        return false;
    }

    out.close();

    std::remove(dataFilePath_.c_str());
    if (std::rename(tempPath.c_str(), dataFilePath_.c_str()) != 0) {
        errorMessage = "Failed to finalize parking data file on disk.";
        std::remove(tempPath.c_str());
        return false;
    }

    return true;
}

bool ParkingSystem::loadState(std::string& errorMessage) {
    std::ifstream in(dataFilePath_);
    if (!in) {
        return true;
    }

    std::string line;
    if (!std::getline(in, line) || line != DATA_VERSION) {
        errorMessage = "Saved data file has an invalid or unsupported format.";
        return false;
    }

    std::unordered_map<VehicleType, double> loadedRates;
    std::unordered_map<std::string, ParkingSlot> loadedSlots;
    std::unordered_map<std::string, ParkingRecord> loadedActive;
    std::vector<ParkingTransaction> loadedTransactions;

    enum class Section { None, Rates, Slots, Active, Transactions };
    Section section = Section::None;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        if (line == "[RATES]") {
            section = Section::Rates;
            continue;
        }
        if (line == "[SLOTS]") {
            section = Section::Slots;
            continue;
        }
        if (line == "[ACTIVE]") {
            section = Section::Active;
            continue;
        }
        if (line == "[TRANSACTIONS]") {
            section = Section::Transactions;
            continue;
        }
        if (line == "[END]") {
            break;
        }

        const std::vector<std::string> fields = splitFields(line, '|');

        if (section == Section::Rates) {
            if (fields.size() != 2) {
                errorMessage = "Invalid rate entry in saved data file.";
                return false;
            }

            VehicleType type{};
            if (!parseStoredVehicleType(fields[0], type)) {
                errorMessage = "Invalid vehicle type in saved rate data.";
                return false;
            }

            try {
                loadedRates[type] = std::stod(fields[1]);
            } catch (const std::exception&) {
                errorMessage = "Invalid rate value in saved data file.";
                return false;
            }
            continue;
        }

        if (section == Section::Slots) {
            if (fields.size() != 4) {
                errorMessage = "Invalid slot entry in saved data file.";
                return false;
            }

            VehicleType type{};
            SlotStatus status{};
            if (!parseStoredVehicleType(fields[1], type) || !parseStoredSlotStatus(fields[3], status)) {
                errorMessage = "Invalid slot entry in saved data file.";
                return false;
            }

            loadedSlots.emplace(fields[0], ParkingSlot(fields[0], type, fields[2], status));
            continue;
        }

        if (section == Section::Active) {
            if (fields.size() != 4) {
                errorMessage = "Invalid active parking entry in saved data file.";
                return false;
            }

            VehicleType type{};
            if (!parseStoredVehicleType(fields[1], type)) {
                errorMessage = "Invalid active parking entry in saved data file.";
                return false;
            }

            try {
                const std::time_t entryTime = static_cast<std::time_t>(std::stoll(fields[3]));
                loadedActive.emplace(fields[0],
                                     ParkingRecord(fields[0], type, fields[2], fromTimeT(entryTime)));
            } catch (const std::exception&) {
                errorMessage = "Invalid entry time in saved active parking data.";
                return false;
            }
            continue;
        }

        if (section == Section::Transactions) {
            if (fields.size() != 10) {
                errorMessage = "Invalid transaction entry in saved data file.";
                return false;
            }

            VehicleType type{};
            if (!parseStoredVehicleType(fields[1], type)) {
                errorMessage = "Invalid transaction entry in saved data file.";
                return false;
            }

            try {
                const std::time_t entryTime = static_cast<std::time_t>(std::stoll(fields[4]));
                const std::time_t exitTime = static_cast<std::time_t>(std::stoll(fields[5]));
                const long long durationMinutes = std::stoll(fields[6]);
                const int billedHours = std::stoi(fields[7]);
                const double hourlyRate = std::stod(fields[8]);
                const double totalFee = std::stod(fields[9]);

                loadedTransactions.emplace_back(fields[0],
                                                type,
                                                fields[2],
                                                fields[3],
                                                fromTimeT(entryTime),
                                                fromTimeT(exitTime),
                                                durationMinutes,
                                                billedHours,
                                                hourlyRate,
                                                totalFee);
            } catch (const std::exception&) {
                errorMessage = "Invalid transaction values in saved data file.";
                return false;
            }
        }
    }

    if (!in.good() && !in.eof()) {
        errorMessage = "Failed while reading parking data from disk.";
        return false;
    }

    pricingManager_.replaceAllRates(loadedRates);
    slotsById_ = std::move(loadedSlots);
    activeVehiclesByPlate_ = std::move(loadedActive);
    transactionHistory_ = std::move(loadedTransactions);
    return true;
}

bool ParkingSystem::persistState() const {
    std::string errorMessage;
    return saveState(errorMessage);
}

void ParkingSystem::loadPersistedState() {
    std::string errorMessage;
    if (loadState(errorMessage)) {
        return;
    }

    slotsById_.clear();
    activeVehiclesByPlate_.clear();
    transactionHistory_.clear();
    pricingManager_ = PricingManager();

    std::cout << "Warning: Could not load saved parking data (" << errorMessage
              << "). Starting with default settings.\n";
}
