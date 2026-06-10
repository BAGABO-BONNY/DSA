#include "ParkingTransaction.h"

// INSERT: appended to transactionHistory_ vector only after successful exit.
// hourlyRate_ is stored permanently so later price updates cannot change history.
ParkingTransaction::ParkingTransaction(const std::string& plateNumber,
                                       VehicleType vehicleType,
                                       const std::string& slotId,
                                       const std::string& zone,
                                       std::chrono::system_clock::time_point entryTime,
                                       std::chrono::system_clock::time_point exitTime,
                                       long long durationMinutes,
                                       int billedHours,
                                       double hourlyRate,
                                       double totalFee)
    : plateNumber_(plateNumber),
      vehicleType_(vehicleType),
      slotId_(slotId),
      zone_(zone),
      entryTime_(entryTime),
      exitTime_(exitTime),
      durationMinutes_(durationMinutes),
      billedHours_(billedHours),
      hourlyRate_(hourlyRate),
      totalFee_(totalFee) {}

const std::string& ParkingTransaction::getPlateNumber() const { return plateNumber_; }

VehicleType ParkingTransaction::getVehicleType() const { return vehicleType_; }

const std::string& ParkingTransaction::getSlotId() const { return slotId_; }

const std::string& ParkingTransaction::getZone() const { return zone_; }

std::chrono::system_clock::time_point ParkingTransaction::getEntryTime() const { return entryTime_; }

std::chrono::system_clock::time_point ParkingTransaction::getExitTime() const { return exitTime_; }

long long ParkingTransaction::getDurationMinutes() const { return durationMinutes_; }

int ParkingTransaction::getBilledHours() const { return billedHours_; }

double ParkingTransaction::getHourlyRate() const { return hourlyRate_; }

double ParkingTransaction::getTotalFee() const { return totalFee_; }
