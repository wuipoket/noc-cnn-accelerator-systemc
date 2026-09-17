#ifndef AXI_DMA_H
#define AXI_DMA_H

#include <systemc.h>
#include <cstdint>
#include <iostream>
#include "DRAM.h"
#include "MemoryMap.h"

class AXI_DMA : public sc_module {
public:
    SC_HAS_PROCESS(AXI_DMA);

    DRAM* dram;

    enum { MAX_BURST_LEN = 256 };

    enum {
        AXI_ADDR_CYCLE = 1,
        AXI_DATA_CYCLE_PER_BEAT = 1,
        AXI_RESP_CYCLE = 1
    };

    uint64_t dram_read_words;
    uint64_t dram_write_words;

    uint64_t read_transactions;
    uint64_t write_transactions;

    uint64_t max_read_burst_len;
    uint64_t max_write_burst_len;

    uint64_t estimated_read_cycles;
    uint64_t estimated_write_cycles;

    uint64_t image_read_words;
    uint64_t weight_read_words;
    uint64_t bias_read_words;
    uint64_t intermediate_read_words;
    uint64_t output_read_words;
    uint64_t other_read_words;

    uint64_t image_write_words;
    uint64_t weight_write_words;
    uint64_t bias_write_words;
    uint64_t intermediate_write_words;
    uint64_t output_write_words;
    uint64_t other_write_words;

    AXI_DMA(sc_module_name name)
        : sc_module(name),
          dram(nullptr),
          dram_read_words(0),
          dram_write_words(0),
          read_transactions(0),
          write_transactions(0),
          max_read_burst_len(0),
          max_write_burst_len(0),
          estimated_read_cycles(0),
          estimated_write_cycles(0),
          image_read_words(0),
          weight_read_words(0),
          bias_read_words(0),
          intermediate_read_words(0),
          output_read_words(0),
          other_read_words(0),
          image_write_words(0),
          weight_write_words(0),
          bias_write_words(0),
          intermediate_write_words(0),
          output_write_words(0),
          other_write_words(0) {}

    void bind_dram(DRAM* d) {
        dram = d;
    }

    void record_read_breakdown(uint32_t addr, int len) {
        if (addr >= DRAM_IMAGE_BASE && addr < DRAM_WEIGHT_BASE) {
            image_read_words += len;
        } else if (addr >= DRAM_WEIGHT_BASE && addr < DRAM_BIAS_BASE) {
            weight_read_words += len;
        } else if (addr >= DRAM_BIAS_BASE && addr < DRAM_INTERMEDIATE_BASE) {
            bias_read_words += len;
        } else if (addr >= DRAM_INTERMEDIATE_BASE && addr < DRAM_OUTPUT_BASE) {
            intermediate_read_words += len;
        } else if (addr >= DRAM_OUTPUT_BASE) {
            output_read_words += len;
        } else {
            other_read_words += len;
        }
    }

    void record_write_breakdown(uint32_t addr, int len) {
        if (addr >= DRAM_IMAGE_BASE && addr < DRAM_WEIGHT_BASE) {
            image_write_words += len;
        } else if (addr >= DRAM_WEIGHT_BASE && addr < DRAM_BIAS_BASE) {
            weight_write_words += len;
        } else if (addr >= DRAM_BIAS_BASE && addr < DRAM_INTERMEDIATE_BASE) {
            bias_write_words += len;
        } else if (addr >= DRAM_INTERMEDIATE_BASE && addr < DRAM_OUTPUT_BASE) {
            intermediate_write_words += len;
        } else if (addr >= DRAM_OUTPUT_BASE) {
            output_write_words += len;
        } else {
            other_write_words += len;
        }
    }

    void dma_read(uint32_t addr, float* buffer, int len) {
        if (dram == nullptr) {
            std::cerr << "[AXI_DMA] DRAM is not bound." << std::endl;
            return;
        }

        int remaining = len;
        int offset = 0;

        while (remaining > 0) {
            int burst_len = (remaining < MAX_BURST_LEN) ? remaining : MAX_BURST_LEN;
            uint32_t burst_addr = addr + offset * 4;

            dram->burst_read(burst_addr, buffer + offset, burst_len);

            read_transactions++;
            dram_read_words += burst_len;
            record_read_breakdown(burst_addr, burst_len);

            estimated_read_cycles += AXI_ADDR_CYCLE
                                   + burst_len * AXI_DATA_CYCLE_PER_BEAT;

            if ((uint64_t)burst_len > max_read_burst_len) {
                max_read_burst_len = burst_len;
            }

            offset += burst_len;
            remaining -= burst_len;
        }
    }

    void dma_write(uint32_t addr, const float* buffer, int len) {
        if (dram == nullptr) {
            std::cerr << "[AXI_DMA] DRAM is not bound." << std::endl;
            return;
        }

        int remaining = len;
        int offset = 0;

        while (remaining > 0) {
            int burst_len = (remaining < MAX_BURST_LEN) ? remaining : MAX_BURST_LEN;
            uint32_t burst_addr = addr + offset * 4;

            dram->burst_write(burst_addr, buffer + offset, burst_len);

            write_transactions++;
            dram_write_words += burst_len;
            record_write_breakdown(burst_addr, burst_len);

            estimated_write_cycles += AXI_ADDR_CYCLE
                                    + burst_len * AXI_DATA_CYCLE_PER_BEAT
                                    + AXI_RESP_CYCLE;

            if ((uint64_t)burst_len > max_write_burst_len) {
                max_write_burst_len = burst_len;
            }

            offset += burst_len;
            remaining -= burst_len;
        }
    }

    void print_stats() const {
        std::cout << "[AXI_DMA] Max burst length setting = "
                  << MAX_BURST_LEN << " words" << std::endl;

        std::cout << "[AXI_DMA] Estimated read cycles  = "
                  << estimated_read_cycles << std::endl;

        std::cout << "[AXI_DMA] Estimated write cycles = "
                  << estimated_write_cycles << std::endl;

        std::cout << "[AXI_DMA] Estimated total transfer cycles = "
                  << estimated_read_cycles + estimated_write_cycles << std::endl;

        std::cout << "[AXI_DMA] DRAM read words  = " << dram_read_words << std::endl;
        std::cout << "[AXI_DMA] DRAM write words = " << dram_write_words << std::endl;
        std::cout << "[AXI_DMA] DRAM read bytes  = " << dram_read_words * 4 << std::endl;
        std::cout << "[AXI_DMA] DRAM write bytes = " << dram_write_words * 4 << std::endl;

        std::cout << "[AXI_DMA] Read transactions  = " << read_transactions << std::endl;
        std::cout << "[AXI_DMA] Write transactions = " << write_transactions << std::endl;

        if (read_transactions > 0) {
            std::cout << "[AXI_DMA] Avg read burst length  = "
                      << (double)dram_read_words / (double)read_transactions
                      << std::endl;
        }

        if (write_transactions > 0) {
            std::cout << "[AXI_DMA] Avg write burst length = "
                      << (double)dram_write_words / (double)write_transactions
                      << std::endl;
        }

        std::cout << "[AXI_DMA] Max read burst length  = "
                  << max_read_burst_len << std::endl;
        std::cout << "[AXI_DMA] Max write burst length = "
                  << max_write_burst_len << std::endl;

        std::cout << "[AXI_DMA] Image read words        = "
                  << image_read_words << std::endl;
        std::cout << "[AXI_DMA] Weight read words       = "
                  << weight_read_words << std::endl;
        std::cout << "[AXI_DMA] Bias read words         = "
                  << bias_read_words << std::endl;
        std::cout << "[AXI_DMA] Intermediate read words = "
                  << intermediate_read_words << std::endl;
        std::cout << "[AXI_DMA] Output read words       = "
                  << output_read_words << std::endl;
        std::cout << "[AXI_DMA] Other read words        = "
                  << other_read_words << std::endl;

        std::cout << "[AXI_DMA] Image write words        = "
                  << image_write_words << std::endl;
        std::cout << "[AXI_DMA] Weight write words       = "
                  << weight_write_words << std::endl;
        std::cout << "[AXI_DMA] Bias write words         = "
                  << bias_write_words << std::endl;
        std::cout << "[AXI_DMA] Intermediate write words = "
                  << intermediate_write_words << std::endl;
        std::cout << "[AXI_DMA] Output write words       = "
                  << output_write_words << std::endl;
        std::cout << "[AXI_DMA] Other write words        = "
                  << other_write_words << std::endl;
    }
};

#endif