#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#pragma once

struct Args {
    unsigned int cycles;
    unsigned int blocksize;
    unsigned int v2b_block_offset;
    unsigned int tlb_size;
    unsigned int tlb_latency;
    unsigned int memory_latency;
    bool debug;
    const char *trace_file; // File name
    const char *log_file; // File name
    const char *filename; // File name
};

struct Request {
    uint32_t addr;
    uint32_t data;
    int we;
};

struct Result {
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitiveGateCount;
};
