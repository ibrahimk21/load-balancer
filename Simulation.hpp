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

    double genInterArrival() {
        std::exponential_distribution<double> dist(lambda_);
        return dist(rng_);
    }

    double genServiceTime(int serverId) {
        std::exponential_distribution<double> dist(servers_[serverId].getMu());
        return dist(rng_);
    }

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
        int sid = selectServer();
        Server& srv = servers_[sid];

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

        if (srv.hasWaiting()) {
            double arrTime = srv.dequeue();
            double waitTime = currentTime_ - arrTime;
            double svcTime = genServiceTime(serverId);
            srv.addWaitTime(waitTime);
            srv.addServiceTime(svcTime);
            scheduleDeparture(serverId, svcTime);
        } else {
            srv.setBusy(false);
        }
    }

public:
    Simulation(double time, int M, const std::vector<double>& P, double lam,
               const std::vector<int>& Q, const std::vector<double>& Mu)
        : maxTime_(time), numServers_(M), probs_(P), lambda_(lam),
          currentTime_(0.0), lastEventTime_(0.0) {
        
        std::random_device rd;
        rng_.seed(rd());

        for (int i = 0; i < M; i++) {
            servers_.emplace_back(Q[i], Mu[i]);
        }
    }

    void run() {
        scheduleArrival();

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
