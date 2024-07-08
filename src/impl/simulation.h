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

struct Result run_simulation_extended(
        int cycles,
        unsigned tlbSize,
        unsigned tlbLatency,
        unsigned blockSize,
        unsigned v2bBlockOffset,
        unsigned memoryLatency,
        size_t numRequests,
        struct Request requests[numRequests],
        const char *tracefile,
        const char *logfile,
        bool debug
);

#ifdef __cplusplus
}
#endif

struct SimulationConfig {
    int cycles;
    unsigned tlbSize;
    unsigned tlbLatency;
    unsigned blockSize;
    unsigned v2bBlockOffset;
    unsigned memoryLatency;
};

#endif //SC_MAIN_H