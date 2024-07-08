#include <iostream>
#include "tlb.cpp"
#include "ram.cpp"
#include <systemc>
#include "log.hpp"

using namespace sc_core;

/**
 * Class that takes an array of requests and simulates their execution on a TLB and RAM
 */
class RequestWorker : public sc_module {

private:
    Logger log;

    Request *requests;
    unsigned numRequests;

    SimulationConfig config;
    TLB *tlb;
    RAM *ram;

    sc_buffer <uint32_t> tlb_addr_in;
    sc_buffer <uint32_t> tlb_addr_out;
    sc_signal<bool> tlb_enabled;

    sc_buffer <uint32_t> ram_addr_in;
    sc_buffer <uint32_t> ram_data_in;
    sc_buffer<int> ram_we_in;
    sc_buffer <uint32_t> ram_data_out;
    sc_signal<bool> ram_enabled;

    uint32_t reqIndex;

public:
    SC_HAS_PROCESS(RequestWorker);

    RequestWorker(sc_module_name name, Logger log, Request *requests, size_t numRequests, TLB *tlb, RAM *ram)
            : sc_module(name),
              log(log) {
        this->requests = requests;
        this->numRequests = numRequests;
        this->tlb = tlb;
        this->ram = ram;

        this->reqIndex = 0;

        this->tlb->addr_in.bind(this->tlb_addr_in);
        this->tlb->addr_out.bind(this->tlb_addr_out);
        this->tlb->enabled.bind(this->tlb_enabled);

        this->ram->addr_in.bind(this->ram_addr_in);
        this->ram->data_in.bind(this->ram_data_in);
        this->ram->we_in.bind(this->ram_we_in);
        this->ram->enabled.bind(this->ram_enabled);
        this->ram->data_out.bind(this->ram_data_out);

        SC_THREAD(handleRequest);
        sensitive << this->ram_data_out;
        SC_THREAD(handleTLBFinish);
        sensitive << this->tlb_addr_out;
    }

    void trace(sc_trace_file *file) {
        sc_trace(file, this->tlb_addr_in, "tlb_addr_in");
        sc_trace(file, this->tlb_addr_out, "tlb_addr_out");
        sc_trace(file, this->tlb_enabled, "tlb_enabled");
        sc_trace(file, this->ram_addr_in, "ram_addr_in");
        sc_trace(file, this->ram_data_in, "ram_data_in");
        sc_trace(file, this->ram_we_in, "ram_we_in");
        sc_trace(file, this->ram_data_out, "ram_data_out");
        sc_trace(file, this->ram_enabled, "ram_enabled");
    }

private:
    void handleRequest() {
        while (true) {
            if (reqIndex >= numRequests) {
                sc_stop();
                return;
            }

            Request request = requests[reqIndex];
            log.DEBUG("-------- Handle request %zu --------", reqIndex);

            this->tlb_addr_in.write(request.addr);

            this->tlb_enabled.write(true);
            wait(SC_ZERO_TIME);
            this->tlb_enabled.write(false);

            this->reqIndex++;
            wait();
        }
    }

    void handleTLBFinish() {
        while (true) {
            wait();

            uint32_t pfn = this->tlb_addr_out.read();
            Request request = this->requests[this->reqIndex];
            // trigger RAM access
            this->ram_we_in.write(request.we);
            this->ram_data_in.write(request.data);
            this->ram_addr_in.write(pfn);

            this->ram_enabled.write(true);
            wait(SC_ZERO_TIME);
            this->ram_enabled.write(false);
        }
    }
};

