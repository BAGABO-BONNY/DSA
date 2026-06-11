/**
 * InputValidator.h
 * ----------------
 * Centralized input validation and safe console reading.
 *
 * OOP - Abstraction: hides validation rules from menu handlers and business
 * logic so ParkingSystem stays focused on parking operations.
 *
 * Requirement: validate inputs and handle exceptional cases gracefully.
 */
#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include "Types.h"
#include <chrono>
#include <string>

class InputValidator {
public:
    static constexpr int MAX_SLOT_ID_LENGTH = 5;
    static constexpr int PLATE_NUMBER_LENGTH = 8;
    static constexpr int MAX_ZONE_LENGTH = 50;
    static constexpr int MAX_SLOTS_ALLOWED = 1000;
    static constexpr int MAX_PARKING_DAYS = 30;
    static constexpr int MAX_PAST_YEARS = 1;
    static constexpr double MAX_HOURLY_RATE = 1000000.0;

    static bool isNonEmpty(const std::string& value);
    static bool containsOnlySafeText(const std::string& value, std::string& errorMessage);
    static std::string normalizeIdentifier(const std::string& value);

    static bool isValidSlotId(const std::string& slotId, std::string& errorMessage);
    static bool isValidPlateNumber(const std::string& plateNumber, std::string& errorMessage);
    static bool isValidZone(const std::string& zone, std::string& errorMessage);
    static bool isPositiveRate(double rate, std::string& errorMessage);
    static bool isFiniteNumber(double value);

    static bool isValidDate(const std::string& date, std::string& errorMessage);
    static bool isNotFutureDate(const std::string& date, std::string& errorMessage);
    static bool parseDateTime(const std::string& input,
                              std::chrono::system_clock::time_point& out,
                              std::string& errorMessage);
    static bool isExitAfterEntry(std::chrono::system_clock::time_point entry,
                                 std::chrono::system_clock::time_point exit,
                                 std::string& errorMessage);
    static bool isNotFutureTime(std::chrono::system_clock::time_point time,
                                std::string& errorMessage);
    static bool isReasonablePastTime(std::chrono::system_clock::time_point time,
                                     std::string& errorMessage);
    static bool isReasonableParkingDuration(std::chrono::system_clock::time_point entry,
                                            std::chrono::system_clock::time_point exit,
                                            std::string& errorMessage);

    static int readMenuChoice(int minChoice, int maxChoice);
    static std::string readNonEmptyLine(const std::string& prompt);
    static std::string readLine(const std::string& prompt);
    static bool readPositiveRate(double& outRate, const std::string& prompt);
};

#endif
