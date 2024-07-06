#ifndef SC_MAIN_H
#define SC_MAIN_H

#include <stddef.h>
#include "types.h" // Include the types.h file

#ifdef __cplusplus
extern "C" {
#endif

struct Result run_simulation(
        int cycles,
        unsigned tlbSize,
        unsigned tlbLatency,
        unsigned blockSize,
        unsigned v2bBlockOffset,
        unsigned memoryLatency,
        size_t numRequests,
        struct Request requests[numRequests],
        const char *tracefile
);

#ifdef __cplusplus
}
#endif

enum State {
    REQ_FETCH, TLB_FETCH, RAM_FETCH
};

struct SimulationConfig {
    int cycles;
    unsigned tlbSize;
    unsigned tlbLatency;
    unsigned blockSize;
    unsigned v2bBlockOffset;
    unsigned memoryLatency;
};

#endif //SC_MAIN_H