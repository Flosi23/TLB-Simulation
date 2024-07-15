#include <limits.h>
#include <stdio.h>
#include "./impl/simulation.h"
#include "csv_parser.h"
#include "arg_parser.h"
#include "types.h"
#include <sys/stat.h>

void writeResultToCSV(struct Result result, const char *filename, unsigned int tlb_size) {
    // Check if the file is empty
    struct stat st;
    int writeHeader = 0;
    if (stat(filename, &st) != 0 || st.st_size == 0) {
        writeHeader = 1;
    }

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return;
    }

    // Write the header if the file is empty
    if (writeHeader) {
        fprintf(file, "TLBSize,Cycles,Hits,Misses,PrimitiveGatesCount\n");
    }

    // Write the data
    fprintf(file, "%u,%zu,%zu,%zu,%zu\n", tlb_size, result.cycles, result.hits, result.misses,
            result.primitiveGateCount);

    fclose(file);
}

int sc_main(int argc, char *argv[]) {
    struct Args args = parseArgs(argc, argv);

    size_t requestCount;
    struct Request *requests = parseCSVFile(args.filename, &requestCount);

    struct Result result = run_simulation_extended(args.cycles, args.tlb_size, args.tlb_latency,
                                                   args.blocksize,
                                                   args.v2b_block_offset,
                                                   args.memory_latency, requestCount, requests, args.trace_file,
                                                   args.log_file,
                                                   args.debug);

    if (args.result_file != NULL) {
        writeResultToCSV(result, args.result_file, args.tlb_size);
    }
    free(requests);
    return 0;
}