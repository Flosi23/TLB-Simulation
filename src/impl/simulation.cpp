#include <iostream>
#include "simulation.h"
#include "tracefix.hpp"
#include <systemc>

/**
 * This function only exists to verify that compling and executing system_c code works;
 */
int test_compilation() {
    sc_clock clk("clk", sc_time(1, SC_SEC));
    sc_signal<int> a;
    sc_signal<int> b;

    sc_signal<int> out_a;
    sc_signal<int> out_b;
    sc_signal<bool> done;

    a = 289835724;
    b = 219131844;

    SEARCHER searcher("searcher");
    searcher.clk(clk);
    searcher.a(a);
    searcher.b(b);
    searcher.out_a(out_a);
    searcher.out_b(out_b);
    searcher.done(done);

    sc_trace_file *file = sc_create_vcd_trace_file("trace_false");
    sc_trace(file, out_a, "out_a");
    sc_trace(file, out_b, "out_b");
    sc_trace(file, done, "done");

    sc_start(50, SC_SEC);

    std::cout << "out_a = " << out_a.read() << std::endl;
    std::cout << "out_b = " << out_b.read() << std::endl;
    std::cout << "done = " << done.read() << std::endl;

    return 0;
}

extern "C" {
struct Result run_simulation(
        int cycles,
        unsigned tlbSize,
        unsigned blockSize,
        unsigned v2bBlockOffset,
        unsigned memoryLatency,
        size_t numRequests,
        struct Request requests[],
        const char *tracefile
) {
    std::cout << "Hello, World from C++!" << std::endl;

    test_compilation();
    // Empty body
    struct Result res;
    return res;
}
}
