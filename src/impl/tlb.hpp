#include <systemc>
#include <systemc.h>
#include "simulation.h"
#include "log.hpp"

using namespace sc_core;

struct TLBEntry {
    // the virtual page number (meaning the virtual address without the block offset)
    uint32_t vpn;
    // the physical frame address (meaning address of the first byte in the frame the virtual address is mapped to)
    uint32_t pfa;
    // whether the entry is valid
    bool valid;
};

struct VirtualAddress {
    uint32_t vpn;
    uint32_t offset;
};


/**
 * We can assume that we need 150 primitive gates for one addition.
 *
 * Our replacement strategy (which just is calculating the index of the virtual address) consists
 * of splitting the virtual address into virtual page number and offset and then calculating the
 * index by taking the virtual page number modulo the TLB size.
 *
 * We assume that the modulo operation and shifting the virtual page number both require about
 * the same amount of gates as an addition which leads to 300 gates for the replacement strategy.
 */
#define PRIMITIVE_GATES_REPLACEMENT_STRATEGY 300

struct TLB : public sc_module {

private:
    Logger log;
    SimulationConfig config{};

    uint8_t numOffsetBits;
    uint8_t numIndexBits;

    size_t hits;
    size_t misses;

    std::vector <TLBEntry> entries;

public:
    sc_in <uint32_t> addr_in;
    sc_out <uint32_t> addr_out;

    sc_in<bool> enabled;

    SC_HAS_PROCESS(TLB);

    TLB(const sc_module_name &name, Logger log, SimulationConfig config) : sc_module(name), log(log) {
        this->config = config;

        this->numOffsetBits = std::ceil(std::log2(this->config.blockSize));
        this->numIndexBits = std::ceil(std::log2(this->config.tlbSize));

        this->hits = 0;
        this->misses = 0;

        for (size_t i = 0; i < this->config.tlbSize; i++) {
            // initialize all entries as invalid
            this->entries.push_back({0, 0, false});
        }

        SC_THREAD(handleVirtualAddress);
        sensitive << enabled;
    }

    size_t getHits() const {
        return this->hits;
    }

    size_t getMisses() const {
        return this->misses;
    }

    size_t getCacheLineSizeInBits() const {
        /*
         * We need `log_2(blockSize) = b` bits to address the byte offset within the block (page).
         *
         * The cache line size of the TLB can be derived as follows:
         * We need `log_2(tlbSize) = i` bits to address the cache line
         * A virtual address is therefore:
         *
         * | Virtual Page Number | Page Offset |
         * |---------------------|-------------|
         * | 32 - b              | b Bits      |
         *
         * In a cache line, the virtual page number and the corresponding
         * physical page numbers must be stored.
         *
         * We also want to store a valid bit for each entry.
         *
         * So the total required bits per cache line is:
         *
         * | Virtual Page Number       | Page Number  | Valid Bit |
         * |---------------------------|--------------|-----------|
         * | 32 - b Bits               | 32 - b Bits  | 1 Bit     |
         *
         * `cSize = 32 - b + 32 - b + 1 = 64 - 2b + 1` bits are needed.
        **/
        return 64 - 2 * numOffsetBits + 1;
    }

    size_t getSizeInBits() const {
        return this->getCacheLineSizeInBits() * config.tlbSize;
    }

    size_t getPrimitiveGateCount() {
        return this->getPrimitiveGatesForStorage() + PRIMITIVE_GATES_REPLACEMENT_STRATEGY;
    }

private:

    size_t getPrimitiveGatesForStorage() const {
        // We assume that we need 4 primitive gates for storing one bit.
        return this->getSizeInBits() * 4;
    }

    void handleVirtualAddress() {
        while (true) {
            if (!enabled.read()) {
                wait();
                continue;
            }

            uint32_t addr = addr_in.read();
            log.DEBUG("TLB access on addr: %zu", addr);

            VirtualAddress va = translate(addr);
            log.DEBUG("Virtual Address: vpn: %zu, offset: %zu", va.vpn, va.offset);

            /**
             * take the index modulo the TLB size in case that the index is not a power of two
             * and the index bits of the virtual address are larger than the TLB size
             */
            uint32_t tlbIndex = this->getTLBIndex(va);
            log.DEBUG("TLB index: %zu", tlbIndex);
            TLBEntry entry = this->entries[tlbIndex];


            // simulate tlb latency
            if (this->config.tlbLatency > 0) {
                log.DEBUG("TLB latency: %zu cycles", this->config.tlbLatency);
                wait(this->config.tlbLatency, SC_NS);
            }

            if (entry.valid && entry.vpn == va.vpn) {
                log.DEBUG("TLB hit: entryVPN: %zu == vaVPN: %zu", entry.vpn, va.vpn);
                this->hits++;

                log.DEBUG("TLB pfa: %zu", entry.pfa);
                addr_out.write(entry.pfa + va.offset);
            } else {
                log.DEBUG("TLB miss");
                this->misses++;
                // simulate page table lookup
                if (this->config.memoryLatency > 0) {
                    log.DEBUG("Memory latency: %zu cycles", this->config.memoryLatency);
                    wait(this->config.memoryLatency, SC_NS);
                }
                // calculate the physical frame number
                uint32_t pfa = getPhysicalFrameAddress(va);

                // we use a direct-mapped cache -> override any existing entry int the TLB
                this->entries[this->getTLBIndex(va)] = {va.vpn, pfa, true};

                log.DEBUG("TLB pfa: %zu", pfa);
                addr_out.write(pfa + va.offset);
            }

            wait();
        }
    }

    /**
     * Splits the address into tag, index and offset
     */
    VirtualAddress translate(uint32_t addr) const {
        VirtualAddress va{};
        va.offset = addr & ((1 << numOffsetBits) - 1);
        va.vpn = addr >> numOffsetBits;
        return va;
    }

    uint32_t getTLBIndex(VirtualAddress va) const {
        return va.vpn % this->config.tlbSize;
    }

    /**
     * Calculates the physical frame address (the first byte in the frame) for the given virtual address
     */
    uint32_t getPhysicalFrameAddress(VirtualAddress va) const {
        // The Virtual page address points to the first byte in the virtual page.
        // It is calculated by multiplying the virtual page number with the block size -> the first byte in the virtual page
        uint32_t virtualPageAddress = va.vpn * this->config.blockSize;
        return virtualPageAddress + this->config.v2bBlockOffset * this->config.blockSize;
    }
};

