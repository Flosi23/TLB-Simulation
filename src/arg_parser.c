#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h> // For INT_MAX in default values
#include "types.h"

/**
 * Goal: Create a method that parses all the commandline arguments.
 */

// Function prototypes
void printHelp();

static int debug_flag = 0;

struct Args parseArgs(int argc, char *argv[]) {

    // Create a struct with all the options that the user could possibly pass
    static struct option long_options[] = {
            {"cycles",           required_argument, 0,           'c'},
            {"blocksize",        required_argument, 0,           'b'}, // Blocksize
            {"v2b-block-offset", required_argument, 0,           'v'}, // V2B Block Offset
            {"tlb-size",         required_argument, 0,           't'}, // TLB Size
            {"tlb-latency",      required_argument, 0,           'l'}, // TLB Latency
            {"memory-latency",   required_argument, 0,           'm'}, // Memory Latency
            {"tf",               required_argument, 0,           'f'}, // Trace File
            {"lf",               required_argument, 0,           'o'}, // Log file (this argument is not required by the task)
            {"help",             no_argument,       0,           'h'}, // Help
            {"debug",            no_argument,       &debug_flag, 1},
            {"rf",               required_argument, 0,           'r'}, // Will write the result (or all if multiple simulations are run) in a csv file
            {0, 0,                                  0,           0} // Terminator
    };

    int option_index = 0;
    int c;

    // All numbers are signed here to detect if the user enters a negative number (prevent implicit cast to unsigned)
    struct UncheckedArgs {
        int cycles;
        int blocksize;
        int v2b_block_offset;
        int tlb_size;
        int tlb_latency;
        int memory_latency;
        const char *result_file; // File name
        const char *trace_file; // File name
        const char *log_file; // File name
        const char *filename; // File name
    };

    // Initialize the struct with default values
    struct UncheckedArgs args = {
            .cycles = INT_MAX,
            .blocksize = 4096, // 4KiB is the default page size used by some operating systems (e.g. linux x84_64), but it differs (e.g. 16KiB on M1 ARM macOS)
            .v2b_block_offset = 8,
            .tlb_size = 256,
            .tlb_latency = 10,
            .memory_latency = 60,
            .result_file = NULL, // No result file
            .trace_file = NULL, // No tracefile
            .log_file = NULL, // No log file
            .filename = NULL // No default -> required arg
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
            case 'r':
                // Result File
                args.result_file = optarg;
                break;
            case '?':
                // Print help reference
                printf("Refer to --help for further information and usage examples.\n");
                exit(1);
                break;
            default:
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

    if (args.cycles <= 0) {
        fprintf(stderr, "Error: Number of cycles must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (args.blocksize <= 0) {
        fprintf(stderr, "Error: Blocksize must be greater than 0\n");
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

    if (args.v2b_block_offset <= 0) {
        fprintf(stderr, "Error: V2B Block Offset must be greater than 0\n");
        printHelp();
        exit(1);
    }

    // after all checks (e.g. if the user entered a negative number) we can cast the values to unsigned
    struct Args checkedArgs;
    checkedArgs.cycles = args.cycles;
    checkedArgs.blocksize = args.blocksize;
    checkedArgs.v2b_block_offset = args.v2b_block_offset;
    checkedArgs.tlb_size = args.tlb_size;
    checkedArgs.tlb_latency = args.tlb_latency;
    checkedArgs.memory_latency = args.memory_latency;
    checkedArgs.trace_file = args.trace_file;
    checkedArgs.log_file = args.log_file;
    checkedArgs.filename = args.filename;
    checkedArgs.debug = debug_flag;
    checkedArgs.result_file = args.result_file;

    return checkedArgs;
}


void printHelp() {
    // Print all options of the program and example usage
    printf("Usage: tlb-sim [OPTIONS] [INPUT_FILE]\n");
    printf("Options:\n");
    printf("  -h,  --help                Print this help message\n");
    printf("  -c,  --cycles              Number of cycles to simulate\n");
    printf("       --blocksize           Block size in bytes\n");
    printf("       --v2b-block-offset    Number of blocks to offset the virtual address\n");
    printf("       --tlb-size            Number of entries in the TLB\n");
    printf("       --tlb-latency         Latency of the TLB in cycles\n");
    printf("       --memory-latency      Latency of the RAM\n");
    printf("       --tf                  Path to trace file\n");
    printf("       --debug               Prints debug information\n");
    printf("       --lf                  Path to log file\n");
    printf("       --rf                  Path to result file\n");
    printf("  filename                   Path to the input file\n");

    printf("\nExample Usage: tlb-sim -c 1000 --blocksize 4096 --v2b-block-offset 8 --tlb-size 256 --tlb-latency 10 --memory-latency 60 inputfile.csv\n");
}
