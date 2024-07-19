#include "request_worker.hpp"
#include <systemc>
#include "log.hpp"

SimulationConfig
createConfig(int cycles, unsigned tlbSize, unsigned tlbLatency, unsigned blockSize, unsigned v2bBlockOffset,
             unsigned memoryLatency) {
    SimulationConfig config{};
    config.cycles = cycles;
    config.tlbSize = tlbSize;
    config.tlbLatency = tlbLatency;
    config.blockSize = blockSize;
    config.v2bBlockOffset = v2bBlockOffset;
    config.memoryLatency = memoryLatency;
    return config;
}

void logSimulationStart(Logger log, SimulationConfig config, size_t numRequests) {
    log.INFO("-------------- Simulation started ----------------");
    log.INFO("Cycles: %zd", config.cycles);
    log.INFO("TLB Size (entries): %zu", config.tlbSize);
    log.INFO("TLB Latency (cycles): %zu", config.tlbLatency);
    log.INFO("Memory Latency (cycles): %zu", config.memoryLatency);
    log.INFO("Block Size (byte): %zu", config.blockSize);
    log.INFO("V2B Block Offset: %zu", config.v2bBlockOffset);
    log.INFO("Number of Requests: %zu", numRequests);
}

void logSimulationEnd(Logger log, Result res, size_t tlbSizeInBits, size_t tlbCacheLineSizeInBits) {
    log.INFO("-------------- Simulation finished ----------------");
    log.INFO("Time (Cycles): %zu", res.cycles);
    log.INFO("TLB Size: %zu bits, %zu byte", tlbSizeInBits, tlbSizeInBits / 8);
    log.INFO("TLB Cache Line Size: %zu bits, %zu byte", tlbCacheLineSizeInBits, tlbCacheLineSizeInBits / 8);
    log.INFO("TLB Hits: %zu", res.hits);
    log.INFO("TLB Misses: %zu", res.misses);
    log.INFO("Primitive Gate Count: %zu", res.primitiveGateCount);
}


extern "C" {

struct Result run_simulation(
        int cycles,
        unsigned tlbSize,
        unsigned tlbLatency,
        unsigned blockSize,
        unsigned v2bBlockOffset,
        unsigned memoryLatency,
        size_t numRequests,
        struct Request *requests,
        const char *tracefile
) {
    return run_simulation_extended(cycles, tlbSize, tlbLatency, blockSize, v2bBlockOffset, memoryLatency, numRequests,
                                   requests, tracefile, nullptr, LogLevel::INFO);
}


struct Result run_simulation_extended(
        int cycles,
        unsigned tlbSize,
        unsigned tlbLatency,
        unsigned blockSize,
        unsigned v2bBlockOffset,
        unsigned memoryLatency,
        size_t numRequests,
        struct Request *requests,
        const char *tracefile,
        const char *logfile,
        bool debug
) {
    sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
                                            sc_core::SC_DO_NOTHING);

    LogLevel logLevel = LogLevel::INFO;
    if (debug) {
        logLevel = LogLevel::DEBUG;
    }
    Logger log(logLevel, logfile);

    // 1 NS = 1 Cycle
    sc_set_default_time_unit(1, SC_NS);
    struct SimulationConfig config = createConfig(cycles, tlbSize, tlbLatency, blockSize, v2bBlockOffset,
                                                  memoryLatency);
    // init all systemc modules
    TLB tlb("TLB", log, config);
    RAM ram("RAM", log, config.memoryLatency);
    RequestWorker requestWorker("Simulation", log, requests, numRequests, &tlb, &ram);

    if (tracefile != nullptr && strlen(tracefile) > 0) {
        sc_trace_file *file = sc_create_vcd_trace_file(tracefile);
        requestWorker.trace(file);
    }

    // start the simulation
    logSimulationStart(log, config, numRequests);
    sc_start(cycles, SC_NS);

    // handle simulation end
    double cyclesElapsed = sc_time_stamp().to_default_time_units();
    struct Result res{};
    // return SIZE_MAX if the simulation did not finished within the given cycles
    res.cycles = requestWorker.getIsFinished() ? (size_t) cyclesElapsed : SIZE_MAX;
    res.misses = tlb.getMisses();
    res.hits = tlb.getHits();
    res.primitiveGateCount = tlb.getPrimitiveGateCount();
    logSimulationEnd(log, res, tlb.getSizeInBits(), tlb.getCacheLineSizeInBits());

    return res;
}

}
