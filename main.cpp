// Load Balancer Simulation
// Simulates M servers with probabilistic routing and finite queues
// Uses discrete-event simulation with exponential inter-arrival and service times

#include <iostream>
#include <vector>
#include <cstdlib>
#include "Simulation.hpp"

void printUsage(const char* prog) {
    std::cerr << "Usage: " << prog << " <Time> <M> <P1..PM> <Lambda> <Q1..QM> <Mu1..MuM>" << std::endl;
}

int main(int argc, char* argv[]) {
    // Command-line parsing
    // Args: <Time> <M> <P1..PM> <Lambda> <Q1..QM> <Mu1..MuM>
    // Total: 4 + 3M arguments (program name + Time + M + M probs + Lambda + M queues + M service rates)
    if (argc < 7) {
        printUsage(argv[0]);
        return 1;
    }

    int idx = 1;
    double simTime = std::atof(argv[idx++]);
    int M = std::atoi(argv[idx++]);

    // Expected total args: 1 + 1 + 1 + M + 1 + M + M = 4 + 3M
    if (argc != 4 + 3 * M) {
        printUsage(argv[0]);
        return 1;
    }

    std::vector<double> P(M);
    for (int i = 0; i < M; i++) {
        P[i] = std::atof(argv[idx++]);
    }

    double lambda = std::atof(argv[idx++]);

    std::vector<int> Q(M);
    for (int i = 0; i < M; i++) {
        Q[i] = std::atoi(argv[idx++]);
    }

    std::vector<double> Mu(M);
    for (int i = 0; i < M; i++) {
        Mu[i] = std::atof(argv[idx++]);
    }

    Simulation sim(simTime, M, P, lambda, Q, Mu);
    sim.run();
    sim.printStats();

    return 0;
}
