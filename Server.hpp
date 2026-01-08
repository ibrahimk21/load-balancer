#ifndef SERVER_HPP
#define SERVER_HPP

#include <queue>

class Server {
private:
    int capacity_;
    bool busy_;
    double mu_;
    std::queue<double> waitQueue_;

    int served_;
    int dropped_;
    double totalWaitTime_;
    double totalServiceTime_;

public:
    Server(int cap, double mu)
        : capacity_(cap), busy_(false), mu_(mu),
          served_(0), dropped_(0), totalWaitTime_(0), totalServiceTime_(0) {}

    bool isBusy() const { return busy_; }
    bool isFull() const { return static_cast<int>(waitQueue_.size()) >= capacity_; }
    bool hasWaiting() const { return !waitQueue_.empty(); }
    double getMu() const { return mu_; }

    void setBusy(bool b) { busy_ = b; }
    void enqueue(double arrivalTime) { waitQueue_.push(arrivalTime); }
    
    double dequeue() {
        double t = waitQueue_.front();
        waitQueue_.pop();
        return t;
    }

    void recordServed() { served_++; }
    void recordDropped() { dropped_++; }
    void addWaitTime(double w) { totalWaitTime_ += w; }
    void addServiceTime(double s) { totalServiceTime_ += s; }

    int getServed() const { return served_; }
    int getDropped() const { return dropped_; }
    double getTotalWaitTime() const { return totalWaitTime_; }
    double getTotalServiceTime() const { return totalServiceTime_; }
};

#endif
