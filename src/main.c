#include <printf.h>
#include "./impl/simulation.h"

int sc_main() {
    struct Request requests[10];
    struct Result result = run_simulation(0, 0, 0, 0, 0, 10, requests, NULL);
    // handle the result here
    printf("Hello World from C");
    return 0;
}