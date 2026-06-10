/**
 * Report.h
 * --------
 * Polymorphic reporting layer.
 *
 * OOP - Inheritance: Report is the abstract base class.
 * OOP - Polymorphism: each concrete report overrides display() and can be used
 * through a Report reference/pointer without knowing the exact report type.
 *
 * TRAVERSAL: each report calls ParkingSystem query methods and prints results.
 */
#ifndef REPORT_H
#define REPORT_H

#include "ParkingSystem.h"

#include <string>

class Report {
public:
    virtual ~Report() = default;

    // Pure virtual methods force concrete report implementations.
    virtual std::string getTitle() const = 0;
    virtual void display(const ParkingSystem& system) const = 0;
};

class AvailableSlotsReport : public Report {
public:
    std::string getTitle() const override;
    void display(const ParkingSystem& system) const override;
};

class ParkedVehiclesReport : public Report {
public:
    std::string getTitle() const override;
    void display(const ParkingSystem& system) const override;
};

class TransactionHistoryReport : public Report {
public:
    std::string getTitle() const override;
    void display(const ParkingSystem& system) const override;
};

#endif
