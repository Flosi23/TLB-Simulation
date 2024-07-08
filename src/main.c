#include "./impl/simulation.h"
#include "csv_parser.h"
#include "arg_parser.h"
#include "types.h"

int sc_main(int argc, char *argv[]) {
    struct Args args = parseArgs(argc, argv);

    size_t requestCount;
    struct Request *requests = parseCSVFile(args.filename, &requestCount);

    run_simulation_extended(args.cycles, args.tlb_size, args.tlb_latency, args.blocksize, args.v2b_block_offset,
                            args.memory_latency, requestCount, requests, args.trace_file, args.log_file, args.debug);

    free(requests);
    return 0;
}