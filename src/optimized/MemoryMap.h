#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <cstdint>

static const uint32_t DRAM_IMAGE_BASE        = 0x00000000;
static const uint32_t DRAM_WEIGHT_BASE       = 0x00100000;
static const uint32_t DRAM_BIAS_BASE         = 0x0F000000;
static const uint32_t DRAM_INTERMEDIATE_BASE = 0x0F100000;
static const uint32_t DRAM_OUTPUT_BASE       = 0x0F200000;

#endif
