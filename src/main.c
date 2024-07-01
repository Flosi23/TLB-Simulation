#include <printf.h>
#include "./impl/simulation.h"

int run_test() {
    size_t numRequests = 3;
    struct Request *requests = calloc(numRequests, sizeof(struct Request));
    if (requests == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return 1;
    }

    struct Request request0;
    request0.addr = 0;
    request0.data = 0;
    request0.we = 0;

    struct Request request1;
    request1.addr = 1;
    request1.data = 0;
    request1.we = 0;

    struct Request request2;
    request2.addr = 2;
    request2.data = 0;
    request2.we = 0;

    requests[0] = request0;
    requests[1] = request1;
    requests[2] = request2;

    int cycles = 1000;
    unsigned tlbSize = 2;
    unsigned tlbLatency = 10;
    unsigned blockSize = 16;
    unsigned v2bBlockOffset = 1;
    unsigned memoryLatency = 20;

    // 1 * 2 = 2

    struct Result result = run_simulation(cycles, tlbSize, tlbLatency, blockSize, v2bBlockOffset, memoryLatency,
                                          numRequests, requests, "test");

    free(requests);
    return 0;
}

int sc_main() {
    run_test();
    return 0;
}