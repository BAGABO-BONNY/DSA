# Kigali Smart Parking Management System

An in-memory Smart Parking Management System for Kigali City, implemented in C++ using **Data Structures and Algorithms (DSA)** and **Object-Oriented Programming (OOP)**.

## System Description

Kigali City faces congestion, poor slot allocation, and revenue leakage because parking is managed manually with paper tickets. This program replaces that process with a console-based smart parking system that:

- configures parking slots in real time
- registers vehicle entry and allocates suitable slots
- tracks parking duration and calculates fees on exit
- releases slots and stores completed transactions
- generates operational reports for attendants and supervisors

All data is stored **in memory only** (no database).

---

## Assessment Requirements Covered

### 1. Linear and Non-Linear Data Structures

| Structure | Type | Where Used | Operations Implemented |
|-----------|------|------------|------------------------|
| `unordered_map<string, ParkingSlot>` | Non-linear (hash table) | `ParkingSystem::slotsById_` | INSERT, UPDATE, LOOKUP, TRAVERSAL |
| `unordered_map<string, ParkingRecord>` | Non-linear (hash table) | `ParkingSystem::activeVehiclesByPlate_` | INSERT, DELETE, LOOKUP, TRAVERSAL |
| `vector<ParkingTransaction>` | Linear (dynamic array) | `ParkingSystem::transactionHistory_` | INSERT (`push_back`), TRAVERSAL |
| `unordered_map<VehicleType, double>` | Non-linear (hash table) | `PricingManager::hourlyRates_` | INSERT, UPDATE, LOOKUP |
| `vector<ParkingSlot>` / `vector<string>` | Linear (temporary result lists) | report/allocation helpers | TRAVERSAL, FILTER, SORT |

### 2. Justification of Data Structure Choices

| Structure | Required Operations | Why This Structure |
|-----------|--------------------|--------------------|
| `unordered_map` for slots | fast lookup by Slot ID, status update on entry/exit | O(1) average lookup/insert/update |
| `unordered_map` for active vehicles | prevent duplicate parking by plate number | O(1) average duplicate detection |
| `vector` for history | append completed transactions, scan for reports | efficient append; simple chronological traversal |
| `unordered_map` for prices | read and update tariffs by vehicle type | O(1) average lookup/update with clean design |

### 3. CRUD Operations in the System

| Operation | Example in Code |
|-----------|----------------|
| **Insertion** | `slotsById_.emplace(...)`, `activeVehiclesByPlate_.emplace(...)`, `transactionHistory_.push_back(...)` |
| **Deletion** | `activeVehiclesByPlate_.erase(...)` when vehicle exits |
| **Update** | `slot.setStatus(...)`, `hourlyRates_[type] = newRate` |
| **Traversal** | loops in `getAllSlots()`, `getTransactionHistory()`, `getDailyRevenue()` |

### 4. OOP Principles Applied

| Principle | Implementation |
|-----------|----------------|
| **Encapsulation** | private fields in `ParkingSlot`, `ParkingRecord`, `ParkingTransaction` |
| **Abstraction** | `ParkingSystem` exposes high-level methods; `InputValidator` hides validation rules |
| **Inheritance** | abstract `Report` base class |
| **Polymorphism** | `AvailableSlotsReport`, `ParkedVehiclesReport`, `TransactionHistoryReport` override `display()` |
| **Composition** | `ParkingSystem` owns `PricingManager`, slot map, active map, and history vector |

### 5. Input Validation and Exceptional Cases

- empty or invalid menu choices (with retry loops)
- invalid control characters in text input
- duplicate slot IDs (case-insensitive: `m-01` = `M-01`)
- invalid slot ID format (max 5 chars; letters, digits, and '-' only; case-insensitive)
- invalid plate number format (`XXX 123X`, e.g. `RAA 123A`)
- invalid zone format (must contain at least one letter or digit)
- maximum slot capacity enforced (1000 slots)
- vehicle already parked (case-insensitive plate matching)
- no slots configured before entry
- no suitable slot available for vehicle type
- preferred slot invalid, missing, occupied, or wrong type
- invalid date format for revenue reports (`YYYY-MM-DD`)
- invalid date-time format (`YYYY-MM-DD HH:MM` or `HH:MM`)
- invalid calendar dates (e.g. `2026-02-30`)
- hour/minute range checks (00-23 / 00-59)
- future time rejected
- entry time too far in the past rejected
- exit before entry rejected
- excessive parking duration rejected (max 30 days)
- invalid, zero, negative, NaN, or unrealistic parking rates
- truck exit blocked until rate is configured (menu 10)
- exit requested for vehicle not currently parked
- revenue calculation overflow protection
- EOF / bad console stream handled safely
- persistent storage: slots, active sessions, transaction history, and rates saved to `parking_data.dat` after each change and on exit

All failures return clear error messages instead of crashing.

### 6. Menu-Driven Interface and Test Inputs

The program uses a console menu in `main.cpp`.

**Quick demo:** choose menu option `12`.

**Manual test inputs:**

| Step | Menu | Sample Input |
|------|------|--------------|
| Configure motorcycle slot | 1 | `M-01`, `Motorcycle`, `Zone A` |
| Configure car slot | 1 | `C-01`, `Car`, `Zone A` |
| Configure truck slot | 1 | `T-01`, `Truck`, `Zone C` |
| Register car entry | 4 | `RAA 123A`, `Car`, blank preferred slot, `08:00` |
| Try duplicate entry | 4 | `RAA 123A` again → should fail |
| Register truck when full | 4 | park one truck, then try second truck → should fail |
| Process exit | 5 | `RAA 123A`, `09:20` → billed as 2 hours |
| Update car price | 10 | `Car`, `1200` |
| View history | 7 | shows stored rate per transaction |
| View revenue | 9 | blank date = today |

---

## Features by Task

- **Task 1:** Configure parking slots (unique ID, vehicle type, zone, status)
- **Task 2:** Register vehicle entry with automatic or preferred slot allocation
- **Task 3:** Calculate parking duration and fees with updatable tariffs
- **Task 4:** Process vehicle exit, release slots, and store transaction history
- **Reports:** Available slots, parked vehicles, transaction history, vehicle history, daily revenue

## Default Parking Rates

Only the tariffs stated in the project brief are pre-loaded at startup:

| Vehicle Type | Hourly Rate |
|--------------|-------------|
| Motorcycle   | 500 RWF     |
| Car          | 1,000 RWF   |

Truck slots and truck parking are supported, but **no default truck tariff is set**. Configure a truck rate at runtime using menu option **10** before processing a truck exit.

### Billing Rules

- Fees are calculated only when a vehicle exits
- Partial hours are charged as full hours (ceiling rule)
  - 15 minutes → 1 hour
  - 1 hour 20 minutes → 2 hours
- Price updates apply only to future exits
- Completed transactions permanently store the rate used at exit time

## Compile and Run

### Windows (MinGW / g++)

```bash
cd c:\Users\admin\Desktop\DSA
g++ -std=c++17 -Wall -Wextra -Iinclude src/Types.cpp src/ParkingSlot.cpp src/ParkingRecord.cpp src/ParkingTransaction.cpp src/PricingManager.cpp src/InputValidator.cpp src/ParkingSystem.cpp src/Report.cpp src/main.cpp -o smart_parking.exe
smart_parking.exe
```

### Linux / macOS

```bash
make
./smart_parking
```

## Menu Options

```
1.  Configure Parking Slot
2.  View All Parking Slots
3.  View Available Slots
4.  Register Vehicle Entry
5.  Process Vehicle Exit
6.  View Currently Parked Vehicles
7.  View Parking Transaction History
8.  View Vehicle History by Plate
9.  View Daily Revenue Report
10. Update Parking Prices
11. View Current Parking Rates
12. Run Demo Test Scenario
0.  Exit
```

## How to Use Each Menu Option

| Option | Purpose | What to Enter |
|--------|---------|---------------|
| 1 | Add a new slot | Slot ID, vehicle type, zone |
| 2 | View every slot | no input |
| 3 | View only free slots | no input |
| 4 | Park a vehicle | plate, type, optional preferred slot, entry time |
| 5 | Exit and pay | plate, exit time |
| 6 | See active parked vehicles | no input |
| 7 | See all completed transactions | no input |
| 8 | See one vehicle's history | plate number |
| 9 | See revenue for a day | date or blank for today |
| 10 | Change active tariff | vehicle type, new rate |
| 11 | Show current tariffs | no input |
| 12 | Run automated demo | no input |

## Suggested Manual Test Flow

1. Configure slots: `M-01` (Motorcycle), `C-01` (Car), `T-01` (Truck)
2. Register entry for plate `RAA 123A` (Car)
3. Try registering `RAA 123A` again → should fail
4. Register a second Truck when only one truck slot exists → should fail gracefully
5. Process exit after 1 hour 20 minutes → billed as 2 hours
6. Update Car price, complete another exit, and verify old history keeps old rate
7. Check reports: available slots, parked vehicles, history, daily revenue

## Demo Scenario

Choose menu option **12** to automatically load sample slots, entries, exits, price updates, and validation cases for quick demonstration.

## Project Structure

```
DSA/
├── include/
│   ├── Types.h
│   ├── ParkingSlot.h
│   ├── ParkingRecord.h
│   ├── ParkingTransaction.h
│   ├── PricingManager.h
│   ├── InputValidator.h
│   ├── ParkingSystem.h
│   └── Report.h
├── src/
│   ├── Types.cpp
│   ├── ParkingSlot.cpp
│   ├── ParkingRecord.cpp
│   ├── ParkingTransaction.cpp
│   ├── PricingManager.cpp
│   ├── InputValidator.cpp
│   ├── ParkingSystem.cpp
│   ├── Report.cpp
│   └── main.cpp
├── Makefile
└── README.md
```
