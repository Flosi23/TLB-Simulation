#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h> // For INT_MAX in default values

/**
 * TODO: Create a method that parses all the commandline arguments.
 */

// Function prototypes
void printHelp();

void parseArgs(int argc, char *argv[]) {
    // Create a struct with all the options that the user could possibly pass
    static struct option long_options[] = {
            {"help",             no_argument,       0, 'h'}, // Help case
            {"blocksize",        optional_argument, 0, 'b'}, // Blocksize
            {"v2b-block-offset", optional_argument, 0, 'v'}, // V2B Block Offset
            {"tlb-size",         optional_argument, 0, 't'}, // TLB Size
            {"tlb-latency",      optional_argument, 0, 'l'}, // TLB Latency
            {"memory-latency",   optional_argument, 0, 'm'}, // Memory Latency
            {"tf",               optional_argument, 0, 'f'}, // Trace File
            {"lf",               optional_argument, 0, 'o'}, // Log file (this argument is not required by the task)
//            {"filename",         required_argument, 0, 'n'}, // TODO: Positional Argument
            {0, 0,                                  0, 0} // Terminator
    };

    // Create a struct to store the parsed values
    struct parsedValues {
        int cycles;
        int blocksize; // Make byte?
        int v2b_block_offset;
        int tlb_size;
        int tlb_latency;
        int memory_latency;
        char *trace_file; // File name
        char *log_file; // File name
        char *filename; // File name
    };

    int option_index = 0;
    int c;

    // Initialize the struct with default values
    struct parsedValues values = {
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
    while ((c = getopt_long(argc, argv, "hvo:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                // Print help
                printHelp();
                break;
            case 'c':
                // Cycles
                values.cycles = atoi(optarg);
                break;
            case 'b':
                // Blocksize
                values.blocksize = atoi(optarg);
                break;
            case 'v':
                // V2B Block Offset
                values.v2b_block_offset = atoi(optarg);
                break;
            case 't':
                // TLB Size
                values.tlb_size = atoi(optarg);
                break;
            case 'l':
                // TLB Latency
                values.tlb_latency = atoi(optarg);
                break;
            case 'm':
                // Memory Latency
                values.memory_latency = atoi(optarg);
                break;
            case 'f':
                // Trace File
                values.trace_file = optarg;
                break;
            case 'o':
                // Log File
                values.log_file = optarg;
                break;
//            case 'n':
//                // Filename
//                values.filename = optarg;
//                break;
            default:
                // Print help reference
                printf("Refer to --help for further information and usage examples.\n");
                break;
        }
    }

    // TODO: Let this work with different orders of args!?
    if (optind < argc) {
        values.filename = argv[optind];
    } else {
        fprintf(stderr, "Error: No filename provided\n");
        printHelp();
        exit(1);
    }


    // Check if optional values make sense
    // TODO: I will use printHelp() for all the values. Should we print a more detailed tailored message?

    if (values.cycles <= 0) {
        fprintf(stderr, "Error: Number of cycles must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (values.blocksize <= 0) {
        fprintf(stderr, "Error: Block size must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (values.tlb_size <= 0) {
        fprintf(stderr, "Error: TLB size must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (values.tlb_latency <= 0) {
        fprintf(stderr, "Error: TLB latency must be greater than 0\n");
        printHelp();
        exit(1);
    }

    if (values.memory_latency <= 0) {
        fprintf(stderr, "Error: Memory latency must be greater than 0\n");
        printHelp();
        exit(1);
    }
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

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    return 0;
}