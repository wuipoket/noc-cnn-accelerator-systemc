#include "systemc.h"
#include "clockreset.h"
#include "AlexNet.h"
#include "Pattern.h"
#include "ROM.h"
#include <cstdlib>
#include <string>

using namespace std;

int sc_main(int argc, char* argv[])
{
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<bool> in_valid;
    sc_signal<bool> out_valid;

    sc_signal<int> layer_id;
    sc_signal<bool> layer_id_type;
    sc_signal<bool> layer_id_valid;
    sc_signal<float> rom_data;
    sc_signal<bool> rom_data_valid;

    sc_vector<sc_signal<double> > img("img", 150528);
    sc_vector<sc_signal<double> > output_linear("output_linear", 1000);
    sc_vector<sc_signal<double> > output_softmax("output_softmax", 1000);

    Clock m_clock("m_clock", 10);

    const char* env_file = getenv("IMAGE_FILE_NAME");
    string image_name;

    if (env_file != NULL) {
        image_name = string(env_file);
    } else if (argc >= 2) {
        image_name = string(argv[1]);
    } else {
        image_name = "cat.txt";
    }

    ROM* rom = new ROM("rom");
   AlexNet* alexnet = new AlexNet("alexnet");
   Pattern* pattern = new Pattern("pattern", image_name, alexnet);

    m_clock(clk);

    rom->clk(clk);
    rom->rst(rst);
    rom->layer_id(layer_id);
    rom->layer_id_type(layer_id_type);
    rom->layer_id_valid(layer_id_valid);
    rom->data(rom_data);
    rom->data_valid(rom_data_valid);

    pattern->clock(clk);
    pattern->rst(rst);
    pattern->in_valid(in_valid);
    pattern->out_valid(out_valid);

    pattern->layer_id(layer_id);
    pattern->layer_id_type(layer_id_type);
    pattern->layer_id_valid(layer_id_valid);
    pattern->rom_data(rom_data);
    pattern->rom_data_valid(rom_data_valid);

    alexnet->clock(clk);
    alexnet->rst(rst);
    alexnet->in_valid(in_valid);
    alexnet->out_valid(out_valid);

    for (int i = 0; i < 150528; i++) {
        pattern->img[i](img[i]);
        alexnet->img[i](img[i]);
    }

    for (int i = 0; i < 1000; i++) {
        alexnet->output_linear[i](output_linear[i]);
        alexnet->output_softmax[i](output_softmax[i]);

        pattern->output_linear[i](output_linear[i]);
        pattern->output_softmax[i](output_softmax[i]);
    }

    sc_start();

    return 0;
}
