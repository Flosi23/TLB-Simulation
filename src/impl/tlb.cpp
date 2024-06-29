#include <systemc>
#include <systemc.h>
#include "simulation.h"

using namespace sc_core;

struct TLBEntry {
    // the tag bits of the virtual address
    uint32_t tag;
    // the physical frame address (meaning address of the first byte in the frame the virtual address is mapped to)
    uint32_t pfa;
    // whether the entry is valid
    bool valid;
};

struct VirtualAddress {
    uint32_t tag;
    uint32_t index;
    uint32_t offset;
};

struct TLB : public sc_module {

private:
    SimulationConfig config;

    uint8_t numOffsetBits;
    uint8_t numIndexBits;
    uint8_t numTagBits;

    size_t hits;
    size_t misses;

    TLBEntry *entries;

public:
    sc_in <uint32_t> addr_in;
    sc_out <uint32_t> addr_out;

    sc_in<bool> enabled;

    SC_HAS_PROCESS(TLB);

    TLB(sc_module_name name, SimulationConfig config) : sc_module(name) {
        this->config = config;

        this->numOffsetBits = std::ceil(std::log2(this->config.blockSize));
        this->numIndexBits = std::ceil(std::log2(this->config.tlbSize));
        this->numTagBits = 32 - numOffsetBits - numIndexBits;

        this->hits = 0;
        this->misses = 0;

        this->entries = (TLBEntry *) calloc(this->config.tlbSize, sizeof(TLBEntry));
        if (this->entries == NULL) {
            std::cerr << "Failed to allocate memory for TLB entries" << std::endl;
            exit(1);
        }
        for (int i = 0; i < this->config.tlbSize; i++) {
            // initialize all entries as invalid
            this->entries[i] = {0, 0, false};
        }

        SC_THREAD(handleVirtualAddress);
        sensitive << enabled;
    }

    void cleanup() {
        free(this->entries);
    }

    size_t getHits() {
        return this->hits;
    }

    size_t getMisses() {
        return this->misses;
    }

    size_t getSizeInBits() {
        /*
         * We need `log_2(blockSize) = b` bits to address the byte offset within the block (page).
         *
         * The cache line size of the TLB can be derived as follows:
         * We need `log_2(tlbSize) = i` bits to address the cache line
         * A virtual address is therefore:
         *
         * | Tag Bits        | Index Bits | Page Offset |
         * |-----------------|------------|-------------|
         * | 32 - b - i Bits | i Bits     | b Bits      |
         *
         * In a cache line, the tag bits of the virtual addresses and the corresponding
         * physical page numbers must be stored.
         *
         * We also want to store a valid bit for each entry.
         *
         * So the total required bits per cache line is:
         *
         * | Tag Bits        | Page Number  | Valid Bit |
         * |-----------------|--------------|-----------|
         * | 32 - b - i Bits | 32 - b Bits  | 1 Bit     |
         *
         * `cSize = 32 - b - i + 32 - b + 1 = 64 - 2b - i + 1` bits are needed.
        **/
        size_t cacheLineSize = 64 - 2 * numOffsetBits - numIndexBits + 1;
        return cacheLineSize * config.tlbSize;
    }

private:

    void handleVirtualAddress() {
        while (true) {
            if (!enabled.read()) {
                wait();
                continue;
            }

            uint32_t addr = addr_in.read();
            std::cout << "TLB access on addr: " << addr << std::endl;

            VirtualAddress va = translate(addr);
            std::cout << "Virtual Address: tag: " << va.tag << " index: " << va.index << " offset: " << va.offset
                      << std::endl;

            TLBEntry entry = this->entries[va.index];


            // simulate tlb latency
            if (this->config.tlbLatency > 0) {
                std::cout << "TLB latency: " << this->config.tlbLatency << " cycles" << std::endl;
                wait(this->config.tlbLatency, SC_NS);
            }

            if (entry.valid && entry.tag == va.tag) {
                std::cout << "TLB hit: entryTag: " << entry.tag << " == vaTag: " << va.tag << std::endl;
                this->hits++;

                std::cout << "TLB pfa: " << entry.pfa << std::endl;
                addr_out.write(entry.pfa + va.offset);
            } else {
                std::cout << "TLB miss" << std::endl;
                this->misses++;
                // simulate page table lookup
                if (this->config.memoryLatency > 0) {
                    std::cout << "Memory latency: " << this->config.memoryLatency << " cycles" << std::endl;
                    wait(this->config.memoryLatency, SC_NS);
                }
                // calculate the physical frame number
                uint32_t pfa = getPhysicalFrameAddress(va);

                // override any existing entry int the TLB
                // (LRU policy but because only one entry per index it's effectively just replacing whatever is there)
                this->entries[va.index] = {va.tag, pfa, 1};

                std::cout << "TLB pfa: " << pfa << std::endl;
                addr_out.write(pfa + va.offset);
            }

            wait();
        }
    }

    /**
     * Splits the address into tag, index and offset
     */
    VirtualAddress translate(uint32_t addr) {
        VirtualAddress va;
        va.tag = addr >> (numOffsetBits + numIndexBits);
        va.offset = addr & ((1 << numOffsetBits) - 1);
        va.index = (addr >> numOffsetBits) & ((1 << numIndexBits) - 1);
        return va;
    }

    /**
     * Calculates the physical frame address (the first byte in the frame) for the given virtual address
     */
    uint32_t getPhysicalFrameAddress(VirtualAddress va) {
        // The Virtual page address points to the first byte in the virtual page.
        // This means it consists of all bits except the offset (since the offset points to the byte within the page)
        uint32_t virtualPageAddress = (va.tag << numIndexBits) | va.index;
        return virtualPageAddress + this->config.v2bBlockOffset * this->config.blockSize;
    }
};

