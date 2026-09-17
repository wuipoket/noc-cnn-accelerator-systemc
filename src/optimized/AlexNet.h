#ifndef ALEXNET_H
#define ALEXNET_H

#include <systemc.h>

SC_MODULE(AlexNet)
{
    sc_in_clk clock;
    sc_in<bool> rst;
    sc_in<bool> in_valid;

    sc_vector<sc_in<double>> img;

    sc_out<bool> out_valid;
    sc_vector<sc_out<double>> output_linear;
    sc_vector<sc_out<double>> output_softmax;

    bool done;
    bool loaded;

    double input_img[3][224][224];
    double padded_img[3][227][227];

    double conv1_weight[64][3][11][11];
    double conv1_bias[64];

    double conv1_out[64][55][55];
    double pool1_out[64][27][27];
    double conv2_pad[64][31][31];
    double conv2_weight[192][64][5][5];
    double conv2_bias[192];
    double conv2_out[192][27][27];
    double pool2_out[192][13][13];
    double conv3_pad[192][15][15];
    double conv3_weight[384][192][3][3];
    double conv3_bias[384];
    double conv3_out[384][13][13];
    double conv4_pad[384][15][15];
    double conv4_weight[256][384][3][3];
    double conv4_bias[256];
    double conv4_out[256][13][13];
    double conv5_pad[256][15][15];
    double conv5_weight[256][256][3][3];
    double conv5_bias[256];
    double conv5_out[256][13][13];
    double pool5_out[256][6][6];
    double flatten_out[9216];
    double fc6_weight[4096][9216];
    double fc6_bias[4096];
    double fc6_out[4096];
    double fc7_weight[4096][4096];
    double fc7_bias[4096];
    double fc7_out[4096];
    double fc8_weight[1000][4096];
    double fc8_bias[1000];
    double fc8_out[1000];
    double softmax_out[1000];
    void load_input();
    void load_conv1_weight_bias();
    void conv1();
    void relu1();
    void pool1();
    void load_conv2_weight_bias();
    void conv2();
    void relu2();
    void pool2();
    void load_conv3_weight_bias();
    void conv3();
    void relu3();
    void load_conv4_weight_bias();
    void conv4();
    void relu4();
    void load_conv5_weight_bias();
    void conv5();
    void relu5();
    void pool5();
    void load_fc6_weight_bias();
    void flatten_pool5();
    void fc6();
    void relu6();
    void load_fc7_weight_bias();
    void fc7();
    void relu7();
    void load_fc8_weight_bias();
    void fc8();
    void softmax();
    void run();

    SC_CTOR(AlexNet)
        : img("img", 150528),
          output_linear("output_linear", 1000),
          output_softmax("output_softmax", 1000)
    {
        done = false;
	loaded = false;
        SC_METHOD(run);
        sensitive << clock.neg();
    }
};

#endif
