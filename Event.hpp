#ifndef EVENT_HPP
#define EVENT_HPP

// Event types for the simulation
enum EventType {
    ARRIVAL,
    DEPARTURE
};

// Event structure - placeholder
struct Event {
    double time;
    EventType type;
    int serverId;
};

#endif
