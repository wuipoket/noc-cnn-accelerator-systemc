#ifndef DRAM_H
#define DRAM_H

#include <systemc.h>
#include <vector>
#include <cstdint>
#include <iostream>

class DRAM : public sc_module {
public:
    SC_HAS_PROCESS(DRAM);

    std::vector<float> mem;

    DRAM(sc_module_name name, size_t size_words = 80 * 1024 * 1024)
        : sc_module(name), mem(size_words, 0.0f) {}

    void write_word(uint32_t addr, float data) {
        uint32_t index = addr / 4;
        if (index >= mem.size()) {
            std::cerr << "[DRAM] Write address out of range: 0x"
                      << std::hex << addr << std::dec << std::endl;
            return;
        }
        mem[index] = data;
    }

    float read_word(uint32_t addr) {
        uint32_t index = addr / 4;
        if (index >= mem.size()) {
            std::cerr << "[DRAM] Read address out of range: 0x"
                      << std::hex << addr << std::dec << std::endl;
            return 0.0f;
        }
        return mem[index];
    }

    void burst_read(uint32_t addr, float* buffer, int len) {
        for (int i = 0; i < len; i++) {
            buffer[i] = read_word(addr + i * 4);
        }
    }

    void burst_write(uint32_t addr, const float* buffer, int len) {
        for (int i = 0; i < len; i++) {
            write_word(addr + i * 4, buffer[i]);
        }
    }
};

#endif
