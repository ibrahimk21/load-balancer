CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall
TARGET = simulator

all: $(TARGET)

$(TARGET): main.cpp Event.hpp Server.hpp Simulation.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
