#ifndef EVENT_HPP
#define EVENT_HPP

enum class EventType {
    ARRIVAL,
    DEPARTURE
};

struct Event {
    double time;
    EventType type;
    int serverId;

    Event(double t, EventType et, int sid = -1)
        : time(t), type(et), serverId(sid) {}
};

struct CompareEvent {
    bool operator()(const Event& a, const Event& b) const {
        return a.time > b.time;
    }
};

#endif
