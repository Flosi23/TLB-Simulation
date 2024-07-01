#include <iostream>
#include "request_worker.cpp"
#include <systemc>

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

    struct SimulationConfig config;
    config.tlbSize = tlbSize;
    config.tlbLatency = tlbLatency;
    config.blockSize = blockSize;
    config.v2bBlockOffset = v2bBlockOffset;
    config.memoryLatency = memoryLatency;

    TLB tlb("TLB", config);
    RAM ram("RAM", config.memoryLatency);
    RequestWorker requestWorker("Simulation", requests, numRequests, &tlb, &ram);

    if (tracefile != NULL && strlen(tracefile) > 0) {
        sc_trace_file *file = sc_create_vcd_trace_file(tracefile);
        requestWorker.trace(file);
    }

    sc_start(cycles, SC_NS);

    double t = sc_time_stamp().to_default_time_units();
    std::cout << "-------------- Simulation finished ----------------" << std::endl;
    std::cout << "Time (Cycles): " << t << std::endl;
    std::cout << "TLB Size (entries): " << tlbSize << std::endl;
    std::cout << "TLB Size (bits): " << tlb.getSizeInBits() << std::endl;
    std::cout << "TLB Hits: " << tlb.getHits() << std::endl;
    std::cout << "TLB Misses: " << tlb.getMisses() << std::endl;

    tlb.cleanup();
    // return empty result for now
    struct Result res;
    return res;
}
}