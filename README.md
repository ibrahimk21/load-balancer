# Load Balancer Simulation

An event-driven simulation of M/M/1/N queues with weighted random routing.

## Build

```bash
make
```

## Usage

```bash
./simulator <Time> <M> <P1..PM> <Lambda> <Q1..QM> <Mu1..MuM>
```

### Parameters

- `Time` - Total simulation time
- `M` - Number of servers
- `P1..PM` - Routing probabilities for each server (must sum to 1)
- `Lambda` - Arrival rate
- `Q1..QM` - Queue capacity for each server
- `Mu1..MuM` - Service rate for each server

### Example

```bash
./simulator 1000 2 0.5 0.5 10 5 5 2.0 3.0
```

This runs a simulation with:
- 1000 time units
- 2 servers
- 50% probability routing to each server
- Arrival rate of 10
- Queue capacity of 5 for each server
- Service rates of 2.0 and 3.0

## Output Format

```
TotalServed TotalDropped LastTime AvgWait AvgService
```

All floating point values are displayed with 4 decimal places.
