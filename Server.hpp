#ifndef SERVER_HPP
#define SERVER_HPP

#include <queue>

// Server class - placeholder
class Server {
private:
    int capacity;
    bool busy;
    double mu;
    
public:
    Server(int cap, double serviceRate) 
        : capacity(cap), busy(false), mu(serviceRate) {}
};

#endif
