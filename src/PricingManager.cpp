#include "PricingManager.h"

#include "InputValidator.h"

// INSERT: default tariffs stated in the project brief only.
PricingManager::PricingManager() {
    hourlyRates_[VehicleType::Motorcycle] = 500.0;   // 500 RWF/hour
    hourlyRates_[VehicleType::Car] = 1000.0;        // 1,000 RWF/hour
}

// LOOKUP: O(1) average retrieval from unordered_map.
double PricingManager::getHourlyRate(VehicleType type) const {
    auto it = hourlyRates_.find(type);
    if (it == hourlyRates_.end()) {
        return 0.0;
    }
    return it->second;
}

bool PricingManager::hasHourlyRate(VehicleType type) const {
    return hourlyRates_.find(type) != hourlyRates_.end();
}

// UPDATE: only affects future exits; completed ParkingTransaction objects
// already store their own hourlyRate_ snapshot.
bool PricingManager::updateHourlyRate(VehicleType type, double newRate, std::string& errorMessage) {
    if (!InputValidator::isPositiveRate(newRate, errorMessage)) {
        return false;
    }

    hourlyRates_[type] = newRate;
    return true;
}
