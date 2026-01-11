#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <queue>
#include <random>
#include <iostream>
#include <iomanip>
#include "Event.hpp"
#include "Server.hpp"

class Simulation {
private:
    double maxTime_;
    int numServers_;
    std::vector<double> probs_;
    double lambda_;
    std::vector<Server> servers_;

    std::priority_queue<Event, std::vector<Event>, CompareEvent> eventQueue_;
    std::mt19937 rng_;
    double currentTime_;
    double lastEventTime_;

    // Generate inter-arrival time using exponential distribution
    // Theory: Poisson arrivals with rate λ ⟹ exponential inter-arrival times with mean 1/λ
    double genInterArrival() {
        std::exponential_distribution<double> dist(lambda_);
        return dist(rng_);
    }

    // Generate service time for server i with rate μ_i
    // Service times are exponentially distributed (standard M/M/1/K assumption)
    double genServiceTime(int serverId) {
        std::exponential_distribution<double> dist(servers_[serverId].getMu());
        return dist(rng_);
    }

    // Probabilistic routing based on given probabilities P_i
    // Generate uniform random r ∈ [0,1] and select server i where cumulative prob exceeds r
    int selectServer() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double r = dist(rng_);
        double cumulative = 0.0;
        for (int i = 0; i < numServers_; i++) {
            cumulative += probs_[i];
            if (r <= cumulative) return i;
        }
        return numServers_ - 1;
    }

    void scheduleArrival() {
        double nextTime = currentTime_ + genInterArrival();
        if (nextTime <= maxTime_) {
            eventQueue_.push(Event(nextTime, EventType::ARRIVAL));
        }
    }

    void scheduleDeparture(int serverId, double serviceTime) {
        double departTime = currentTime_ + serviceTime;
        eventQueue_.push(Event(departTime, EventType::DEPARTURE, serverId));
    }

    void handleArrival() {
        scheduleArrival();
        
        // Routing happens FIRST: select target server using probabilities P_i
        int sid = selectServer();
        Server& srv = servers_[sid];

        // Drop condition: packet is dropped if target server is busy AND queue is full
        // Key: routing is deterministic once selected. No fallback to other servers.
        // If server is busy but queue has space, packet joins the wait queue.
        if (srv.isBusy() && srv.isFull()) {
            srv.recordDropped();
            return;
        }

        if (!srv.isBusy()) {
            srv.setBusy(true);
            double svcTime = genServiceTime(sid);
            srv.addWaitTime(0.0);
            srv.addServiceTime(svcTime);
            scheduleDeparture(sid, svcTime);
        } else {
            srv.enqueue(currentTime_);
        }
    }

    void handleDeparture(int serverId) {
        Server& srv = servers_[serverId];
        srv.recordServed();

        // If there are packets waiting in the queue, immediately start serving the next one
        // Wait time = current_time - arrival_time (time spent in queue)
        if (srv.hasWaiting()) {
            double arrTime = srv.dequeue();
            double waitTime = currentTime_ - arrTime;
            double svcTime = genServiceTime(serverId);
            srv.addWaitTime(waitTime);
            srv.addServiceTime(svcTime);
            scheduleDeparture(serverId, svcTime);
        } else {
            // No packets waiting, server becomes idle
            srv.setBusy(false);
        }
    }

public:
    Simulation(double time, int M, const std::vector<double>& P, double lam,
               const std::vector<int>& Q, const std::vector<double>& Mu)
        : maxTime_(time), numServers_(M), probs_(P), lambda_(lam),
          currentTime_(0.0), lastEventTime_(0.0) {
        
        // Use std::random_device for non-deterministic seeding
        // This ensures different runs produce different results (important for testing)
        std::random_device rd;
        rng_.seed(rd());

        for (int i = 0; i < M; i++) {
            servers_.emplace_back(Q[i], Mu[i]);
        }
    }

    void run() {
        scheduleArrival();

        // Event-driven simulation using a priority queue (min-heap)
        // Process events in chronological order until max time is exceeded
        while (!eventQueue_.empty()) {
            Event ev = eventQueue_.top();
            eventQueue_.pop();

            currentTime_ = ev.time;
            lastEventTime_ = currentTime_;

            if (ev.type == EventType::ARRIVAL) {
                handleArrival();
            } else {
                handleDeparture(ev.serverId);
            }
        }
    }

    void printStats() const {
        int totalServed = 0;
        int totalDropped = 0;
        double totalWait = 0.0;
        double totalService = 0.0;

        for (const auto& srv : servers_) {
            totalServed += srv.getServed();
            totalDropped += srv.getDropped();
            totalWait += srv.getTotalWaitTime();
            totalService += srv.getTotalServiceTime();
        }

        // Average wait time = total time spent waiting / number of packets served
        // Note: dropped packets don't contribute to these averages
        double avgWait = (totalServed > 0) ? totalWait / totalServed : 0.0;
        double avgService = (totalServed > 0) ? totalService / totalServed : 0.0;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << totalServed << " "
                  << totalDropped << " "
                  << lastEventTime_ << " "
                  << avgWait << " "
                  << avgService << std::endl;
    }
};

#endif
