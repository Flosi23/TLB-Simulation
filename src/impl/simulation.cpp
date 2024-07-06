#include "request_worker.cpp"
#include <systemc>

SimulationConfig
createConfig(int cycles, unsigned tlbSize, unsigned tlbLatency, unsigned blockSize, unsigned v2bBlockOffset,
             unsigned memoryLatency) {
    SimulationConfig config;
    config.cycles = cycles;
    config.tlbSize = tlbSize;
    config.tlbLatency = tlbLatency;
    config.blockSize = blockSize;
    config.v2bBlockOffset = v2bBlockOffset;
    config.memoryLatency = memoryLatency;
    return config;
}

void logSimulationStart(SimulationConfig config, size_t numRequests) {
    std::cout << "-------------- Simulation started ----------------" << std::endl;
    std::cout << "Cycles: " << config.cycles << std::endl;
    std::cout << "TLB Size (entries): " << config.tlbSize << std::endl;
    std::cout << "TLB Latency (cycles): " << config.tlbLatency << std::endl;
    std::cout << "Memory Latency (cycles): " << config.memoryLatency << std::endl;
    std::cout << "Block Size (byte): " << config.blockSize << std::endl;
    std::cout << "V2B Block Offset: " << config.v2bBlockOffset << std::endl;
    std::cout << "Number of Requests: " << numRequests << std::endl;
}

void logSimulationEnd(Result res, size_t tlbSizeInBits, size_t tlbCacheLineSizeInBits) {
    std::cout << "-------------- Simulation finished ----------------" << std::endl;
    std::cout << "Time (Cycles): " << res.cycles << std::endl;
    std::cout << "TLB Size: " << tlbSizeInBits << " bits, " << tlbSizeInBits / 8 << " byte" << std::endl;
    std::cout << "TLB Cache Line Size: " << tlbCacheLineSizeInBits << "bits, " << tlbCacheLineSizeInBits / 8 << " byte"
              << std::endl;
    std::cout << "TLB Hits: " << res.hits << std::endl;
    std::cout << "TLB Misses: " << res.misses << std::endl;
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
        struct Request requests[],
        const char *tracefile
) {
    sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
                                            sc_core::SC_DO_NOTHING);
    // 1 NS = 1 Cycle
    sc_set_default_time_unit(1, SC_NS);
    struct SimulationConfig config = createConfig(cycles, tlbSize, tlbLatency, blockSize, v2bBlockOffset,
                                                  memoryLatency);
    // init all systemc modules
    TLB tlb("TLB", config);
    RAM ram("RAM", config.memoryLatency);
    RequestWorker requestWorker("Simulation", requests, numRequests, &tlb, &ram);

    if (tracefile != NULL && strlen(tracefile) > 0) {
        sc_trace_file *file = sc_create_vcd_trace_file(tracefile);
        requestWorker.trace(file);
    }

    // start the simulation
    logSimulationStart(config, numRequests);
    sc_start(cycles, SC_NS);

    // handle simulation end
    double cyclesElapsed = sc_time_stamp().to_default_time_units();
    struct Result res;
    res.cycles = cyclesElapsed;
    res.misses = tlb.getMisses();
    res.hits = tlb.getHits();
    logSimulationEnd(res, tlb.getSizeInBits(), tlb.getCacheLineSizeInBits());

    // free up any memory allocated for the TLB
    tlb.cleanup();
    return res;
}
}