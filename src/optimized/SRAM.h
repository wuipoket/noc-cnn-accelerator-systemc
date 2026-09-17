#ifndef SRAM_H
#define SRAM_H

#include <vector>
#include <cstdint>
#include <iostream>

class SRAM {
public:
    std::vector<float> mem;
    uint64_t read_words;
    uint64_t write_words;
    uint64_t estimated_read_cycles;
    uint64_t estimated_write_cycles;
    uint32_t capacity_words;

    enum {
        SRAM_READ_CYCLE = 1,
        SRAM_WRITE_CYCLE = 1
    };

    SRAM(uint32_t capacity = 64 * 1024)
        : mem(capacity, 0.0f),
          read_words(0),
          write_words(0),
          estimated_read_cycles(0),
          estimated_write_cycles(0),
          capacity_words(capacity) {}

    void write_word(uint32_t index, float data) {
        if (index >= capacity_words) {
            std::cerr << "[SRAM] Write index out of range: "
                      << index << std::endl;
            return;
        }
        mem[index] = data;
        write_words++;
        estimated_write_cycles += SRAM_WRITE_CYCLE;
    }

    float read_word(uint32_t index) {
        if (index >= capacity_words) {
            std::cerr << "[SRAM] Read index out of range: "
                      << index << std::endl;
            return 0.0f;
        }
        read_words++;
        estimated_read_cycles += SRAM_READ_CYCLE;
        return mem[index];
    }

    void load_from_buffer(const float* buffer, int len) {
        if ((uint32_t)len > capacity_words) {
            std::cerr << "[SRAM] Buffer length exceeds SRAM capacity." << std::endl;
            return;
        }

        for (int i = 0; i < len; i++) {
            write_word(i, buffer[i]);
        }
    }

    void store_to_buffer(float* buffer, int len) {
        if ((uint32_t)len > capacity_words) {
            std::cerr << "[SRAM] Buffer length exceeds SRAM capacity." << std::endl;
            return;
        }

        for (int i = 0; i < len; i++) {
            buffer[i] = read_word(i);
        }
    }

    void print_stats() const {
        std::cout << "[SRAM] Capacity words = " << capacity_words << std::endl;
        std::cout << "[SRAM] Capacity bytes = " << capacity_words * 4 << std::endl;
        std::cout << "[SRAM] Read words     = " << read_words << std::endl;
        std::cout << "[SRAM] Write words    = " << write_words << std::endl;
        std::cout << "[SRAM] Read bytes     = " << read_words * 4 << std::endl;
        std::cout << "[SRAM] Write bytes    = " << write_words * 4 << std::endl;
        std::cout << "[SRAM] Estimated read cycles  = "
                  << estimated_read_cycles << std::endl;

	std::cout << "[SRAM] Estimated write cycles = "
                  << estimated_write_cycles << std::endl;

	std::cout << "[SRAM] Estimated total cycles = "
                  << estimated_read_cycles + estimated_write_cycles << std::endl;
    }
};

#endif
