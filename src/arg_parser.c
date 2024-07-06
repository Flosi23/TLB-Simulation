#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h> // For INT_MAX in default values
#include "types.h"

/**
 * TODO: Create a method that parses all the commandline arguments.
 */

// Function prototypes
void printHelp();

struct Args parseArgs(int argc, char *argv[]) {
    // Create a struct with all the options that the user could possibly pass
    static struct option long_options[] = {
            {"cycles",           required_argument, 0, 'c'},
            {"blocksize",        required_argument, 0, 'b'}, // Blocksize
            {"v2b-block-offset", required_argument, 0, 'v'}, // V2B Block Offset
            {"tlb-size",         required_argument, 0, 't'}, // TLB Size
            {"tlb-latency",      required_argument, 0, 'l'}, // TLB Latency
            {"memory-latency",   required_argument, 0, 'm'}, // Memory Latency
            {"tf",               required_argument, 0, 'f'}, // Trace File
            {"lf",               required_argument, 0, 'o'}, // Log file (this argument is not required by the task)
            {"help",             no_argument,       0, 'h'}, // Help
            {0,                  0,                 0, 0} // Terminator
    };

    int option_index = 0;
    int c;

    // Initialize the struct with default values
    struct Args args = {
            .cycles = INT_MAX,
            .blocksize = 512, // 512 Byte is the smallest found value in RISC_V systems
            .v2b_block_offset = 8, // 8 is simply a power of 2 which is a common value
            .tlb_size = 512, // This is the number of entries in the TLB
            .tlb_latency = 1, // TODO: NOT FINAL
            .memory_latency = 100, // TODO: NOT FINAL
            .trace_file = NULL, // No tracefile
            .log_file = NULL, // No log file
            .filename = NULL // No default because of required arg
    };

    // Parse the arguments
    while ((c = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                // Print help
                printHelp();
                exit(0);
            case 'c':
                // Cycles
                args.cycles = atoi(optarg);
                break;
            case 'b':
                // Blocksize
                args.blocksize = atoi(optarg);
                break;
            case 'v':
                // V2B Block Offset
                args.v2b_block_offset = atoi(optarg);
                break;
            case 't':
                // TLB Size
                args.tlb_size = atoi(optarg);
                break;
            case 'l':
                // TLB Latency
                args.tlb_latency = atoi(optarg);
                break;
            case 'm':
                // Memory Latency
                args.memory_latency = atoi(optarg);
                break;
            case 'f':
                // Trace File
                args.trace_file = optarg;
                break;
            case 'o':
                // Log File
                args.log_file = optarg;
                break;
            default:
                // Print help reference
                printf("Refer to --help for further information and usage examples.\n");
                break;
        }
    }

    // TODO: Let this work with different orders of args!?
    if (optind < argc) {
        args.filename = argv[optind];
    } else {
        fprintf(stderr, "Error: No filename provided\n");
        printHelp();
        exit(1);
    }

    // Check if optional values make sense
    // TODO: I will use printHelp() for all the values. Should we print a more detailed tailored message?

    if (args.cycles <= 0) {
        fprintf(stderr, "Error: Number of cycles must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (args.blocksize <= 0) {
        fprintf(stderr, "Error: Block size must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (args.tlb_size <= 0) {
        fprintf(stderr, "Error: TLB size must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (args.tlb_latency <= 0) {
        fprintf(stderr, "Error: TLB latency must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (args.memory_latency <= 0) {
        fprintf(stderr, "Error: Memory latency must be greater than 0\n");
        printHelp();
        exit(1);
    }

    return args;
}


void printHelp() {
    // TODO: Print all options of the program and example usage
    printf("Usage: tlb-sim [OPTIONS] Input_File [filename...]\n");
    printf("Options:\n");
    printf("  -h, --help                Print this help message\n");
    printf("  -c, --cycles              Number of cycles to simulate\n");
    printf("  -b, --blocksize           Block size in bytes\n");
    printf("  -v, --v2b-block-offset    Number of blocks to offset the virtual address\n");
    printf("  -t, --tlb-size            Number of entries in the TLB\n");
    printf("  -l, --tlb-latency         Latency of the TLB in cycles\n");
    printf("  -m, --memory-latency      Latency of the RAM\n");
    printf("  -f, --tf                  Path to trace file\n");
    printf("  filename                  Path to the input file\n"); // TODO: Change if necessary


    // TODO: Example usage
}
