#include <systemc>
#include "../types.h"
#include "log.hpp"

using namespace sc_core;

struct RAM : public sc_module {

private:
    Logger log;

    unsigned memoryLatency;

public:
    sc_in <uint32_t> data_in;
    sc_in<int> we_in;
    sc_in <uint32_t> addr_in;

    sc_in<bool> enabled;

    sc_out <uint32_t> data_out;


    SC_HAS_PROCESS(RAM);

    RAM(sc_module_name name, Logger log, unsigned memoryLatency) : sc_module(name), log(log) {
        this->memoryLatency = memoryLatency;
        SC_THREAD(handleRequest);
        sensitive << enabled;
    }

    // handles a request from the CPU and simulates the memory latency
    void handleRequest() {
        while (true) {
            if (!enabled.read()) {
                wait();
                continue;
            }

            uint32_t addr = addr_in.read();
            uint32_t data = data_in.read();
            int we = we_in.read();

            log.DEBUG("Memory Access on: addr: %zu, data: %zu, we: %d", addr, data, we);
            // simulate memory Latency if > 0
            if (this->memoryLatency > 0) {
                log.DEBUG("Memory Latency: %zu cycles", this->memoryLatency);
                wait(this->memoryLatency, SC_NS);
            }
            // send the data back to the CPU
            // TODO: actually read and write data from memory?
            data_out.write(addr);

            wait();
        }
    }

};