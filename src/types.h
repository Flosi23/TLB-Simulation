#include <stdint.h>
#include <stdlib.h>

#pragma once

struct Args {
    uint32_t cycles;
    uint32_t blocksize;
    int32_t v2b_block_offset;
    uint32_t tlb_size;
    uint32_t tlb_latency;
    uint32_t memory_latency;
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
