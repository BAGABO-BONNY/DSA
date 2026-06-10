/**
 * ParkingTransaction.h
 * ----------------------
 * Immutable completed parking record created only at vehicle exit.
 *
 * OOP - Encapsulation: stores the hourly rate used at exit time so later price
 * updates cannot change historical billing (Task 3 requirement).
 *
 * Task 4: preserves transaction details for reports and vehicle history.
 */
#ifndef PARKING_TRANSACTION_H
#define PARKING_TRANSACTION_H

#include "Types.h"
#include <chrono>
#include <string>

class ParkingTransaction {
public:
    ParkingTransaction(const std::string& plateNumber,
                       VehicleType vehicleType,
                       const std::string& slotId,
                       const std::string& zone,
                       std::chrono::system_clock::time_point entryTime,
                       std::chrono::system_clock::time_point exitTime,
                       long long durationMinutes,
                       int billedHours,
                       double hourlyRate,
                       double totalFee);

    const std::string& getPlateNumber() const;
    VehicleType getVehicleType() const;
    const std::string& getSlotId() const;
    const std::string& getZone() const;
    std::chrono::system_clock::time_point getEntryTime() const;
    std::chrono::system_clock::time_point getExitTime() const;
    long long getDurationMinutes() const;
    int getBilledHours() const;
    double getHourlyRate() const;
    double getTotalFee() const;

private:
    std::string plateNumber_;
    VehicleType vehicleType_;
    std::string slotId_;
    std::string zone_;
    std::chrono::system_clock::time_point entryTime_;
    std::chrono::system_clock::time_point exitTime_;
    long long durationMinutes_;
    int billedHours_;       // Ceiling-hour billing result
    double hourlyRate_;     // Rate frozen at exit time
    double totalFee_;
};

#endif
