#ifndef COMPUTE_MODEL_H
#define COMPUTE_MODEL_H

#include <iostream>
#include <cstdint>

class ComputeModel {
public:
    enum {
        NUM_PES = 16,
        MAC_UNITS_PER_PE = 16,
        TOTAL_MAC_UNITS = NUM_PES * MAC_UNITS_PER_PE
    };

    static uint64_t ceil_div(uint64_t a, uint64_t b) {
        return (a + b - 1) / b;
    }

    static uint64_t conv_ops(uint64_t out_h,
                             uint64_t out_w,
                             uint64_t out_c,
                             uint64_t in_c,
                             uint64_t k_h,
                             uint64_t k_w) {
        return out_h * out_w * out_c * in_c * k_h * k_w;
    }

    static uint64_t fc_ops(uint64_t in_features,
                           uint64_t out_features) {
        return in_features * out_features;
    }

    static uint64_t total_mac_operations() {
        uint64_t conv1 = conv_ops(55, 55, 64, 3, 11, 11);
        uint64_t conv2 = conv_ops(27, 27, 192, 64, 5, 5);
        uint64_t conv3 = conv_ops(13, 13, 384, 192, 3, 3);
        uint64_t conv4 = conv_ops(13, 13, 256, 384, 3, 3);
        uint64_t conv5 = conv_ops(13, 13, 256, 256, 3, 3);

        uint64_t fc6 = fc_ops(9216, 4096);
        uint64_t fc7 = fc_ops(4096, 4096);
        uint64_t fc8 = fc_ops(4096, 1000);

        return conv1 + conv2 + conv3 + conv4 + conv5 + fc6 + fc7 + fc8;
    }

    static uint64_t estimated_compute_cycles() {
        return ceil_div(total_mac_operations(), TOTAL_MAC_UNITS);
    }

    static void print_stats() {
        std::cout << "[COMPUTE] Number of PEs = "
                  << NUM_PES << std::endl;

        std::cout << "[COMPUTE] MAC units per PE = "
                  << MAC_UNITS_PER_PE << std::endl;

        std::cout << "[COMPUTE] Total MAC units = "
                  << TOTAL_MAC_UNITS << std::endl;

        std::cout << "[COMPUTE] Total MAC operations = "
                  << total_mac_operations() << std::endl;

        std::cout << "[COMPUTE] Estimated compute cycles = "
                  << estimated_compute_cycles() << std::endl;
    }
};

#endif
