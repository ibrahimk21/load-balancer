#!/bin/bash

# Compile first
echo "Compiling..."
make
echo "Compilation finished."
echo ""

# Helper function to parse and print results
print_results() {
    # Capture the output of the command passed as arguments
    output=$($@)
    
    # Read the space-separated output into variables
    read served dropped time wait service <<< "$output"
    
    # Print clearly
    echo "  -> [RAW OUTPUT]: $output"
    echo "  -> ANALYSIS:"
    echo "     * Total Requests Served:  $served"
    echo "     * Total Dropped Packets:  $dropped"
    echo "     * Final Simulation Time:  $time"
    echo "     * Avg Wait Time (Queue):  $wait"
    echo "     * Avg Service Time:       $service"
    echo ""
}

echo "================================================"
echo "          LOAD BALANCER SIMULATION TESTS        "
echo "================================================"
echo ""

echo "TEST 1: Single Server Stability Check (M/M/1)"
echo "Input: Time=5000, Lambda=20, Mu=40 (Rho=0.5)"
echo "Expected: ~100k served, ~0 drops, low wait time."
print_results ./simulator 5000 1 1 20 1000 40
echo "------------------------------------------------"

echo "TEST 2: Routing Logic Check (4 Servers, 100% to Server 1)"
echo "Input: 4 Servers, but P1=1.0 (others unused)."
echo "Expected: Results should match Test 1 almost exactly."
print_results ./simulator 5000 4 1 0 0 0 20 1000 1000 1000 1000 40 40 40 40
echo "------------------------------------------------"

echo "TEST 3: Zero Queue Capacity (Pure Loss System)"
echo "Input: Queue=0. If server busy -> Drop immediately."
echo "Expected: High drop rate (approx 33% theoretically)."
print_results ./simulator 5000 1 1 20 0 40
echo "------------------------------------------------"

echo "TEST 4: Overloaded System (Unstable)"
echo "Input: Lambda=50, Mu=40. (Arrivals > Service Rate)"
echo "Expected: Server 100% busy. Queue fills up. Massive drops."
print_results ./simulator 5000 1 1 50 100 40
echo "------------------------------------------------"

echo "TEST 5: Distributed Load (Balanced)"
echo "Input: 2 Servers, 50/50 split, Lambda=40, Mu=40 each."
echo "Expected: Load balanced. Total served ~200k (combined capacity)."
print_results ./simulator 5000 2 0.5 0.5 40 100 100 40 40
echo "------------------------------------------------"

echo "Removing .exe file"
make clean