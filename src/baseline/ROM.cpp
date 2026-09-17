#include "ROM.h"
#include <sstream>
DRAM* g_dram = nullptr;
AXI_DMA* g_dma = nullptr;
int ROM::get_data_count(int id, bool type)
{
    if (id == 0) return 3 * 224 * 224;

    if (id == 1) return (type == 0) ? 64 * 3 * 11 * 11 : 64;
    if (id == 2) return (type == 0) ? 192 * 64 * 5 * 5 : 192;
    if (id == 3) return (type == 0) ? 384 * 192 * 3 * 3 : 384;
    if (id == 4) return (type == 0) ? 256 * 384 * 3 * 3 : 256;
    if (id == 5) return (type == 0) ? 256 * 256 * 3 * 3 : 256;
    if (id == 6) return (type == 0) ? 4096 * 9216 : 4096;
    if (id == 7) return (type == 0) ? 4096 * 4096 : 4096;
    if (id == 8) return (type == 0) ? 1000 * 4096 : 1000;

    return 0;
}

uint32_t ROM::get_base_addr(int id, bool type)
{
    if (id == 0) {
        return DRAM_IMAGE_BASE;
    }

    uint32_t addr = (type == 0) ? DRAM_WEIGHT_BASE : DRAM_BIAS_BASE;

    for (int layer = 1; layer < id; layer++) {
        addr += get_data_count(layer, type) * 4;
    }

    return addr;
}

void ROM::load_file_to_dram(const string& filename, uint32_t base_addr, int expected_count)
{
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cerr << "Error: Failed to open file during DRAM initialization: "
             << filename << endl;
        sc_stop();
        return;
    }

    float value;
    int count = 0;

    while (count < expected_count && (file >> value)) {
        dram->write_word(base_addr + count * 4, value);
        count++;
    }

    file.close();

    if (count != expected_count) {
        cerr << "Error: Unexpected data count in "
             << filename
             << ". Expected "
             << expected_count
             << ", got "
             << count
             << endl;
        sc_stop();
    }
}

void ROM::initialize_dram()
{
    if (dram_initialized) {
        return;
    }

    load_file_to_dram(DATA_PATH + IMAGE_FILE_NAME,
                      DRAM_IMAGE_BASE,
                      get_data_count(0, 0));

    for (int id = 1; id <= 5; id++) {
        stringstream ss;
        ss << id;

        load_file_to_dram(DATA_PATH + "conv" + ss.str() + "_weight.txt",
                          get_base_addr(id, 0),
                          get_data_count(id, 0));

        load_file_to_dram(DATA_PATH + "conv" + ss.str() + "_bias.txt",
                          get_base_addr(id, 1),
                          get_data_count(id, 1));
    }

    for (int id = 6; id <= 8; id++) {
        stringstream ss;
        ss << id;

        load_file_to_dram(DATA_PATH + "fc" + ss.str() + "_weight.txt",
                          get_base_addr(id, 0),
                          get_data_count(id, 0));

        load_file_to_dram(DATA_PATH + "fc" + ss.str() + "_bias.txt",
                          get_base_addr(id, 1),
                          get_data_count(id, 1));
    }

    dram_initialized = true;

    cout << "[DRAM] Initialization completed." << endl;
}

void ROM::run()
{
    int id;
    bool type;
    uint32_t base_addr;

    initialize_dram();

    while (rst.read()) {
        wait();
    }

    while (true)
    {
        if (!is_reading_data)
        {
            data_valid.write(false);
            data.write(0);

            if (layer_id_valid.read())
            {
                id = layer_id.read();
                type = layer_id_type.read();

                current_len = get_data_count(id, type);
                base_addr = get_base_addr(id, type);

                if (current_len <= 0) {
                    cout << "Error: Invalid layer id " << id << "." << endl;
                    sc_stop();
                }

                current_buffer.assign(current_len, 0.0f);
                dma->dma_read(base_addr, current_buffer.data(), current_len);

                current_idx = 0;
                is_reading_data = true;
            }
        }
        else
        {
            if (layer_id_valid.read())
            {
                cout << "Error: layer_id_valid should be low when reading data." << endl;
                sc_stop();
            }

            if (current_idx < current_len)
            {
                data_valid.write(true);
                data.write(current_buffer[current_idx]);
                current_idx++;
            }
            else
            {
                data_valid.write(false);
                data.write(0);
                current_buffer.clear();
                current_len = 0;
                current_idx = 0;
                is_reading_data = false;
            }
        }

        wait();
    }
}
