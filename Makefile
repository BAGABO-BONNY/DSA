CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude
SRC = src/Types.cpp src/ParkingSlot.cpp src/ParkingRecord.cpp src/ParkingTransaction.cpp \
      src/PricingManager.cpp src/InputValidator.cpp src/ParkingSystem.cpp src/Report.cpp src/main.cpp
TARGET = smart_parking

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) smart_parking.exe

.PHONY: all run clean
