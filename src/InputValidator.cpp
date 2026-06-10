#include "InputValidator.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace {

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) {
        return 0;
    }
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

bool hasOnlyAllowedDateChars(const std::string& value) {
    for (char c : value) {
        if (!(std::isdigit(static_cast<unsigned char>(c)) || c == '-')) {
            return false;
        }
    }
    return true;
}

}  // namespace

bool InputValidator::isNonEmpty(const std::string& value) {
    return !value.empty();
}

bool InputValidator::containsOnlySafeText(const std::string& value, std::string& errorMessage) {
    for (unsigned char c : value) {
        if (std::iscntrl(c)) {
            errorMessage = "Input contains invalid control characters.";
            return false;
        }
    }
    return true;
}

std::string InputValidator::normalizeIdentifier(const std::string& value) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return normalized;
}

bool InputValidator::isValidSlotId(const std::string& slotId, std::string& errorMessage) {
    if (!isNonEmpty(slotId)) {
        errorMessage = "Slot ID cannot be empty.";
        return false;
    }

    if (!containsOnlySafeText(slotId, errorMessage)) {
        return false;
    }

    if (slotId.length() > MAX_SLOT_ID_LENGTH) {
        errorMessage = "Slot ID must not exceed " + std::to_string(MAX_SLOT_ID_LENGTH) + " characters.";
        return false;
    }

    if (!std::isalnum(static_cast<unsigned char>(slotId.front()))) {
        errorMessage = "Slot ID must start with a letter or digit.";
        return false;
    }

    for (char c : slotId) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') {
            errorMessage = "Slot ID may contain only letters, numbers, '-' or '_'.";
            return false;
        }
    }

    return true;
}

bool InputValidator::isValidPlateNumber(const std::string& plateNumber, std::string& errorMessage) {
    if (!isNonEmpty(plateNumber)) {
        errorMessage = "Plate number cannot be empty.";
        return false;
    }

    if (!containsOnlySafeText(plateNumber, errorMessage)) {
        return false;
    }

    if (plateNumber.length() < MIN_PLATE_LENGTH || plateNumber.length() > MAX_PLATE_LENGTH) {
        errorMessage = "Plate number must be between " + std::to_string(MIN_PLATE_LENGTH) +
                       " and " + std::to_string(MAX_PLATE_LENGTH) + " characters.";
        return false;
    }

    bool hasLetter = false;
    bool hasDigit = false;

    for (char c : plateNumber) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            hasLetter = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
        } else if (c != '-' && c != ' ') {
            errorMessage = "Plate number may contain only letters, digits, spaces, or '-'.";
            return false;
        }
    }

    if (!hasLetter || !hasDigit) {
        errorMessage = "Plate number must contain both letters and digits.";
        return false;
    }

    return true;
}

bool InputValidator::isValidZone(const std::string& zone, std::string& errorMessage) {
    if (!isNonEmpty(zone)) {
        errorMessage = "Zone cannot be empty.";
        return false;
    }

    if (!containsOnlySafeText(zone, errorMessage)) {
        return false;
    }

    if (zone.length() > MAX_ZONE_LENGTH) {
        errorMessage = "Zone name must not exceed " + std::to_string(MAX_ZONE_LENGTH) + " characters.";
        return false;
    }

    bool hasAlphaNumeric = false;
    for (char c : zone) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            hasAlphaNumeric = true;
            break;
        }
    }

    if (!hasAlphaNumeric) {
        errorMessage = "Zone name must contain at least one letter or digit.";
        return false;
    }

    return true;
}

bool InputValidator::isFiniteNumber(double value) {
    return std::isfinite(value);
}

bool InputValidator::isPositiveRate(double rate, std::string& errorMessage) {
    if (!isFiniteNumber(rate)) {
        errorMessage = "Parking rate must be a valid finite number.";
        return false;
    }

    if (rate <= 0.0) {
        errorMessage = "Parking rate must be greater than zero.";
        return false;
    }

    if (rate > MAX_HOURLY_RATE) {
        errorMessage = "Parking rate is unreasonably high. Please enter a realistic value.";
        return false;
    }

    return true;
}

bool InputValidator::isValidDate(const std::string& date, std::string& errorMessage) {
    if (!isNonEmpty(date)) {
        errorMessage = "Date cannot be empty.";
        return false;
    }

    if (!containsOnlySafeText(date, errorMessage)) {
        return false;
    }

    if (!hasOnlyAllowedDateChars(date)) {
        errorMessage = "Date must use only digits and '-' in YYYY-MM-DD format.";
        return false;
    }

    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        errorMessage = "Invalid date format. Use YYYY-MM-DD.";
        return false;
    }

    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail() || !ss.eof()) {
        errorMessage = "Invalid date format. Use YYYY-MM-DD.";
        return false;
    }

    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;

    if (year < 2000 || year > 2100) {
        errorMessage = "Year must be between 2000 and 2100.";
        return false;
    }

    if (month < 1 || month > 12) {
        errorMessage = "Month must be between 01 and 12.";
        return false;
    }

    int maxDay = daysInMonth(year, month);
    if (day < 1 || day > maxDay) {
        errorMessage = "Day is invalid for the given month and year.";
        return false;
    }

    return true;
}

bool InputValidator::parseDateTime(const std::string& input,
                                   std::chrono::system_clock::time_point& out,
                                   std::string& errorMessage) {
    if (!isNonEmpty(input)) {
        errorMessage = "Date-time cannot be empty.";
        return false;
    }

    if (!containsOnlySafeText(input, errorMessage)) {
        return false;
    }

    std::tm tm = {};
    std::istringstream ss(input);
    bool hasFullDate = false;

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (!ss.fail()) {
        hasFullDate = true;
        std::string remaining;
        ss >> remaining;
        if (!remaining.empty()) {
            errorMessage = "Invalid date-time format. Use YYYY-MM-DD HH:MM or HH:MM.";
            return false;
        }
    } else {
        ss.clear();
        ss.str(input);
        ss >> std::get_time(&tm, "%H:%M");
        if (ss.fail()) {
            errorMessage = "Invalid date-time format. Use YYYY-MM-DD HH:MM or HH:MM.";
            return false;
        }

        std::string remaining;
        ss >> remaining;
        if (!remaining.empty()) {
            errorMessage = "Invalid date-time format. Use YYYY-MM-DD HH:MM or HH:MM.";
            return false;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm* localNow = std::localtime(&nowTime);
        if (!localNow) {
            errorMessage = "Unable to read current local time.";
            return false;
        }

        tm.tm_year = localNow->tm_year;
        tm.tm_mon = localNow->tm_mon;
        tm.tm_mday = localNow->tm_mday;
    }

    if (tm.tm_hour < 0 || tm.tm_hour > 23) {
        errorMessage = "Hour must be between 00 and 23.";
        return false;
    }

    if (tm.tm_min < 0 || tm.tm_min > 59) {
        errorMessage = "Minute must be between 00 and 59.";
        return false;
    }

    if (hasFullDate) {
        int year = tm.tm_year + 1900;
        int month = tm.tm_mon + 1;
        int day = tm.tm_mday;

        if (year < 2000 || year > 2100) {
            errorMessage = "Year must be between 2000 and 2100.";
            return false;
        }

        if (month < 1 || month > 12) {
            errorMessage = "Month must be between 01 and 12.";
            return false;
        }

        int maxDay = daysInMonth(year, month);
        if (day < 1 || day > maxDay) {
            errorMessage = "Day is invalid for the given month and year.";
            return false;
        }
    }

    tm.tm_isdst = -1;
    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        errorMessage = "Invalid date-time value.";
        return false;
    }

    out = std::chrono::system_clock::from_time_t(time);
    return true;
}

bool InputValidator::isExitAfterEntry(std::chrono::system_clock::time_point entry,
                                        std::chrono::system_clock::time_point exit,
                                        std::string& errorMessage) {
    if (exit < entry) {
        errorMessage = "Exit time cannot be earlier than entry time.";
        return false;
    }
    return true;
}

bool InputValidator::isNotFutureTime(std::chrono::system_clock::time_point time,
                                     std::string& errorMessage) {
    auto now = std::chrono::system_clock::now();
    if (time > now + std::chrono::minutes(1)) {
        errorMessage = "Time cannot be in the future.";
        return false;
    }
    return true;
}

bool InputValidator::isReasonablePastTime(std::chrono::system_clock::time_point time,
                                          std::string& errorMessage) {
    auto now = std::chrono::system_clock::now();
    auto oldestAllowed = now - std::chrono::hours(24 * 365 * MAX_PAST_YEARS);

    if (time < oldestAllowed) {
        errorMessage = "Time is too far in the past. Please enter a more recent value.";
        return false;
    }

    return true;
}

bool InputValidator::isReasonableParkingDuration(std::chrono::system_clock::time_point entry,
                                                 std::chrono::system_clock::time_point exit,
                                                 std::string& errorMessage) {
    auto duration = std::chrono::duration_cast<std::chrono::hours>(exit - entry);
    if (duration.count() > 24 * MAX_PARKING_DAYS) {
        errorMessage = "Parking duration exceeds the maximum allowed period of " +
                       std::to_string(MAX_PARKING_DAYS) + " days.";
        return false;
    }

    return true;
}

int InputValidator::readMenuChoice(int minChoice, int maxChoice) {
    while (true) {
        std::cout << "Enter choice (" << minChoice << "-" << maxChoice << "): ";
        int choice = 0;

        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                std::cout << "\nInput ended unexpectedly. Returning to safe option.\n";
                std::cin.clear();
                return minChoice;
            }

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a whole number.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice < minChoice || choice > maxChoice) {
            std::cout << "Choice out of range. Please enter a value between "
                      << minChoice << " and " << maxChoice << ".\n";
            continue;
        }

        return choice;
    }
}

std::string InputValidator::readNonEmptyLine(const std::string& prompt) {
    while (true) {
        std::string value = readLine(prompt);
        if (isNonEmpty(value)) {
            return value;
        }
        std::cout << "Input cannot be empty. Please try again.\n";
    }
}

std::string InputValidator::readLine(const std::string& prompt) {
    std::cout << prompt;

    if (!std::cin.good()) {
        std::cin.clear();
    }

    std::string value;
    if (!std::getline(std::cin, value)) {
        if (std::cin.eof()) {
            std::cout << "\nInput stream closed. Using empty input.\n";
            return "";
        }
        std::cin.clear();
        return "";
    }

    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");

    if (start == std::string::npos) {
        return "";
    }

    return value.substr(start, end - start + 1);
}

bool InputValidator::readPositiveRate(double& outRate, const std::string& prompt) {
    while (true) {
        std::string input = readLine(prompt);

        if (!isNonEmpty(input)) {
            std::cout << "Rate cannot be empty. Please try again.\n";
            continue;
        }

        std::istringstream ss(input);
        double rate = 0.0;
        ss >> rate;

        if (ss.fail() || !ss.eof()) {
            std::cout << "Invalid rate. Enter a numeric value such as 1000 or 1000.50.\n";
            continue;
        }

        std::string error;
        if (!isPositiveRate(rate, error)) {
            std::cout << "Error: " << error << "\n";
            continue;
        }

        outRate = rate;
        return true;
    }
}
