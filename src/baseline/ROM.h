#ifndef ROM_H
#define ROM_H

#include "systemc.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "DRAM.h"
#include "AXI_DMA.h"
#include "MemoryMap.h"
extern DRAM* g_dram;
extern AXI_DMA* g_dma;
using namespace std;

SC_MODULE( ROM ) {
    sc_in  < bool >  clk;
    sc_in  < bool >  rst;

    sc_in  < int >   layer_id;
    sc_in  < bool >  layer_id_type;
    sc_in  < bool >  layer_id_valid;

    sc_out < float > data;
    sc_out < bool >  data_valid;

    DRAM* dram;
    AXI_DMA* dma;

    vector<float> current_buffer;
    int current_len;
    int current_idx;
    bool is_reading_data;
    bool dram_initialized;

    void run();
    void initialize_dram();
    void load_file_to_dram(const string& filename, uint32_t base_addr, int expected_count);
    uint32_t get_base_addr(int id, bool type);
    int get_data_count(int id, bool type);

    string DATA_PATH;
    string IMAGE_FILE_NAME;

    SC_CTOR( ROM )
    {
        DATA_PATH = "./data/";

        const char* env_file = getenv("IMAGE_FILE_NAME");
        if (env_file != NULL) {
            IMAGE_FILE_NAME = env_file;
        } else {
            IMAGE_FILE_NAME = "cat.txt";
        }

        dram = new DRAM("dram");
	dma = new AXI_DMA("axi_dma");
	dma->bind_dram(dram);

	g_dram = dram;
	g_dma = dma;

        current_len = 0;
        current_idx = 0;
        is_reading_data = false;
        dram_initialized = false;

        SC_THREAD( run );
        sensitive << clk.pos() << rst.neg();
    }

    ~ROM()
    {
        delete dma;
        delete dram;
    }
};

#endif
