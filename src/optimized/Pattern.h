#ifndef PATTERN_H
#define PATTERN_H

#include <systemc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include "AlexNet.h"

#define CYCLE 70000000

using namespace std;

SC_MODULE(Pattern)
{
    sc_in_clk clock;
    sc_in<bool> out_valid;

    sc_vector<sc_in<double> > output_softmax;
    sc_vector<sc_in<double> > output_linear;

    sc_out<bool> rst, in_valid;
    sc_vector<sc_out<double> > img;

    sc_out<int> layer_id;
    sc_out<bool> layer_id_type;
    sc_out<bool> layer_id_valid;
    sc_in<float> rom_data;
    sc_in<bool> rom_data_valid;

    string img_name;
    unsigned long long cycle;

    AlexNet* net;

    int stage;
    int load_state;
    int elem_idx;
    bool network_started;

    void run();

    int get_layer_id(int s);
    bool get_layer_type(int s);
    int get_stage_count(int s);
    void store_stage_data(int s, int idx, double value);

    SC_HAS_PROCESS(Pattern);

    Pattern(sc_module_name name, string img_name, AlexNet* alexnet)
        : sc_module(name),
          output_softmax("output_softmax", 1000),
          output_linear("output_linear", 1000),
          img("img", 150528),
          img_name(img_name),
          net(alexnet)
    {
        cycle = 0;
        stage = 0;
        load_state = 0;
        elem_idx = 0;
        network_started = false;

        SC_METHOD(run);
        sensitive << clock.neg();
    }
};

#endif
