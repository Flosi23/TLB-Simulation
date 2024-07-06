#include "./impl/simulation.h"
#include "csv_parser.h"
#include "arg_parser.h"
#include "types.h"
#include <stdio.h>

int sc_main(int argc, char *argv[]) {
    struct Args args = parseArgs(argc, argv);

    size_t requestCount;
    struct Request *requests = parseCSVFile(args.filename, &requestCount);

    printf("%zu Requests parsed\n", requestCount);
    for (size_t i = 0; i < requestCount; i++) {
        printf("Request %zu: Type: %s, Address: 0x%08X, Data: 0x%08X\n",
               i + 1, requests[i].we ? "Write" : "Read", requests[i].addr, requests[i].data);
    }

    free(requests);
    return 0;
}