#include "AlexNet.h"
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

void AlexNet::load_input()
{
    int idx = 0;
    for (int c = 0; c < 3; c++)
    {
        for (int h = 0; h < 224; h++)
        {
            for (int w = 0; w < 224; w++)
            {
                input_img[c][h][w] = img[idx].read();
                idx++;
            }
        }
    }

    for (int c = 0; c < 3; c++)
    {
        for (int h = 0; h < 227; h++)
        {
            for (int w = 0; w < 227; w++)
            {
                padded_img[c][h][w] = 0.0;
            }
        }
    }

    for (int c = 0; c < 3; c++)
    {
    	for (int h = 0; h < 224; h++)
        {	
            for (int w = 0; w < 224; w++)
            {  
                padded_img[c][h + 2][w + 2] = input_img[c][h][w];
            }  
        } 
    }	
}

void AlexNet::load_conv1_weight_bias()
{
   
}

void AlexNet::conv1()
{
    for (int oc = 0; oc < 64; oc++)
    {
        for (int oh = 0; oh < 55; oh++)
        {
            for (int ow = 0; ow < 55; ow++)
            {
                double sum = conv1_bias[oc];

                for (int ic = 0; ic < 3; ic++)
                {
                    for (int kh = 0; kh < 11; kh++)
                    {
                        for (int kw = 0; kw < 11; kw++)
                        {
                            int ih = oh * 4 + kh;
                            int iw = ow * 4 + kw;
                            sum += padded_img[ic][ih][iw] * conv1_weight[oc][ic][kh][kw];
                        }
                    }
                }

                conv1_out[oc][oh][ow] = sum;
            }
        }
    }
}

void AlexNet::relu1()
{
    for (int oc = 0; oc < 64; oc++)
    {
        for (int h = 0; h < 55; h++)
        {
            for (int w = 0; w < 55; w++)
            {
                if (conv1_out[oc][h][w] < 0.0)
                    conv1_out[oc][h][w] = 0.0;
            }
        }
    }
}
void AlexNet::pool1()
{
    for (int c = 0; c < 64; c++)
    {
        for (int h = 0; h < 27; h++)
        {
            for (int w = 0; w < 27; w++)
            {
                double mx = conv1_out[c][h * 2][w * 2];

                for (int kh = 0; kh < 3; kh++)
                {
                    for (int kw = 0; kw < 3; kw++)
                    {
                        int ih = h * 2 + kh;
                        int iw = w * 2 + kw;
                        if (conv1_out[c][ih][iw] > mx)
                            mx = conv1_out[c][ih][iw];
                    }
                }

                pool1_out[c][h][w] = mx;
            }
        }
    }
}
void AlexNet::load_conv2_weight_bias()
{

}

void AlexNet::conv2()
{
    for (int c = 0; c < 64; c++)
    {
        for (int h = 0; h < 31; h++)
        {
            for (int w = 0; w < 31; w++)
            {
                conv2_pad[c][h][w] = 0.0;
            }
        }
    }

    for (int c = 0; c < 64; c++)
    {
        for (int h = 0; h < 27; h++)
        {
            for (int w = 0; w < 27; w++)
            {
                conv2_pad[c][h + 2][w + 2] = pool1_out[c][h][w];
            }
        }
    }

    for (int oc = 0; oc < 192; oc++)
    {
        for (int oh = 0; oh < 27; oh++)
        {
            for (int ow = 0; ow < 27; ow++)
            {
                double sum = conv2_bias[oc];

                for (int ic = 0; ic < 64; ic++)
                {
                    for (int kh = 0; kh < 5; kh++)
                    {
                        for (int kw = 0; kw < 5; kw++)
                        {
                            int ih = oh + kh;
                            int iw = ow + kw;
                            sum += conv2_pad[ic][ih][iw] * conv2_weight[oc][ic][kh][kw];
                        }
                    }
                }

                conv2_out[oc][oh][ow] = sum;
            }
        }
    }
}

void AlexNet::relu2()
{
    for (int oc = 0; oc < 192; oc++)
    {
        for (int h = 0; h < 27; h++)
        {
            for (int w = 0; w < 27; w++)
            {
                if (conv2_out[oc][h][w] < 0.0)
                    conv2_out[oc][h][w] = 0.0;
            }
        }
    }
}

void AlexNet::pool2()
{
    for (int c = 0; c < 192; c++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                double mx = conv2_out[c][h * 2][w * 2];

                for (int kh = 0; kh < 3; kh++)
                {
                    for (int kw = 0; kw < 3; kw++)
                    {
                        int ih = h * 2 + kh;
                        int iw = w * 2 + kw;
                        if (conv2_out[c][ih][iw] > mx)
                            mx = conv2_out[c][ih][iw];
                    }
                }

                pool2_out[c][h][w] = mx;
            }
        }
    }
}

void AlexNet::load_conv3_weight_bias()
{
   
}

void AlexNet::conv3()
{
    for (int c = 0; c < 192; c++)
    {
        for (int h = 0; h < 15; h++)
        {
            for (int w = 0; w < 15; w++)
            {
                conv3_pad[c][h][w] = 0.0;
            }
        }
    }

    for (int c = 0; c < 192; c++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                conv3_pad[c][h + 1][w + 1] = pool2_out[c][h][w];
            }
        }
    }

    for (int oc = 0; oc < 384; oc++)
    {
        for (int oh = 0; oh < 13; oh++)
        {
            for (int ow = 0; ow < 13; ow++)
            {
                double sum = conv3_bias[oc];

                for (int ic = 0; ic < 192; ic++)
                {
                    for (int kh = 0; kh < 3; kh++)
                    {
                        for (int kw = 0; kw < 3; kw++)
                        {
                            int ih = oh + kh;
                            int iw = ow + kw;
                            sum += conv3_pad[ic][ih][iw] * conv3_weight[oc][ic][kh][kw];
                        }
                    }
                }

                conv3_out[oc][oh][ow] = sum;
            }
        }
    }
}

void AlexNet::relu3()
{
    for (int oc = 0; oc < 384; oc++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                if (conv3_out[oc][h][w] < 0.0)
                    conv3_out[oc][h][w] = 0.0;
            }
        }
    }
}

void AlexNet::load_conv4_weight_bias()
{
  
}

void AlexNet::conv4()
{
    for (int c = 0; c < 384; c++)
    {
        for (int h = 0; h < 15; h++)
        {
            for (int w = 0; w < 15; w++)
            {
                conv4_pad[c][h][w] = 0.0;
            }
        }
    }

    for (int c = 0; c < 384; c++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                conv4_pad[c][h + 1][w + 1] = conv3_out[c][h][w];
            }
        }
    }

    for (int oc = 0; oc < 256; oc++)
    {
        for (int oh = 0; oh < 13; oh++)
        {
            for (int ow = 0; ow < 13; ow++)
            {
                double sum = conv4_bias[oc];

                for (int ic = 0; ic < 384; ic++)
                {
                    for (int kh = 0; kh < 3; kh++)
                    {
                        for (int kw = 0; kw < 3; kw++)
                        {
                            int ih = oh + kh;
                            int iw = ow + kw;
                            sum += conv4_pad[ic][ih][iw] * conv4_weight[oc][ic][kh][kw];
                        }
                    }
                }

                conv4_out[oc][oh][ow] = sum;
            }
        }
    }
}

void AlexNet::relu4()
{
    for (int oc = 0; oc < 256; oc++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                if (conv4_out[oc][h][w] < 0.0)
                    conv4_out[oc][h][w] = 0.0;
            }
        }
    }
}

void AlexNet::load_conv5_weight_bias()
{
  
}

void AlexNet::conv5()
{
    for (int c = 0; c < 256; c++)
    {
        for (int h = 0; h < 15; h++)
        {
            for (int w = 0; w < 15; w++)
            {
                conv5_pad[c][h][w] = 0.0;
            }
        }
    }

    for (int c = 0; c < 256; c++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                conv5_pad[c][h + 1][w + 1] = conv4_out[c][h][w];
            }
        }
    }

    for (int oc = 0; oc < 256; oc++)
    {
        for (int oh = 0; oh < 13; oh++)
        {
            for (int ow = 0; ow < 13; ow++)
            {
                double sum = conv5_bias[oc];

                for (int ic = 0; ic < 256; ic++)
                {
                    for (int kh = 0; kh < 3; kh++)
                    {
                        for (int kw = 0; kw < 3; kw++)
                        {
                            int ih = oh + kh;
                            int iw = ow + kw;
                            sum += conv5_pad[ic][ih][iw] * conv5_weight[oc][ic][kh][kw];
                        }
                    }
                }

                conv5_out[oc][oh][ow] = sum;
            }
        }
    }
}

void AlexNet::relu5()
{
    for (int oc = 0; oc < 256; oc++)
    {
        for (int h = 0; h < 13; h++)
        {
            for (int w = 0; w < 13; w++)
            {
                if (conv5_out[oc][h][w] < 0.0)
                    conv5_out[oc][h][w] = 0.0;
            }
        }
    }
}

void AlexNet::pool5()
{
    for (int c = 0; c < 256; c++)
    {
        for (int h = 0; h < 6; h++)
        {
            for (int w = 0; w < 6; w++)
            {
                double mx = conv5_out[c][h * 2][w * 2];

                for (int kh = 0; kh < 3; kh++)
                {
                    for (int kw = 0; kw < 3; kw++)
                    {
                        int ih = h * 2 + kh;
                        int iw = w * 2 + kw;
                        if (conv5_out[c][ih][iw] > mx)
                            mx = conv5_out[c][ih][iw];
                    }
                }

                pool5_out[c][h][w] = mx;
            }
        }
    }
}

void AlexNet::load_fc6_weight_bias()
{
 
}

void AlexNet::flatten_pool5()
{
    int idx = 0;
    for (int c = 0; c < 256; c++)
    {
        for (int h = 0; h < 6; h++)
        {
            for (int w = 0; w < 6; w++)
            {
                flatten_out[idx] = pool5_out[c][h][w];
                idx++;
            }
        }
    }
}

void AlexNet::fc6()
{
    for (int o = 0; o < 4096; o++)
    {
        double sum = fc6_bias[o];
        for (int i = 0; i < 9216; i++)
        {
            sum += flatten_out[i] * fc6_weight[o][i];
        }
        fc6_out[o] = sum;
    }
}

void AlexNet::relu6()
{
    for (int i = 0; i < 4096; i++)
    {
        if (fc6_out[i] < 0.0)
            fc6_out[i] = 0.0;
    }
}


void AlexNet::load_fc7_weight_bias()
{
   
}

void AlexNet::fc7()
{
    for (int o = 0; o < 4096; o++)
    {
        double sum = fc7_bias[o];
        for (int i = 0; i < 4096; i++)
        {
            sum += fc6_out[i] * fc7_weight[o][i];
        }
        fc7_out[o] = sum;
    }
}

void AlexNet::relu7()
{
    for (int i = 0; i < 4096; i++)
    {
        if (fc7_out[i] < 0.0)
            fc7_out[i] = 0.0;
    }
}


void AlexNet::load_fc8_weight_bias()
{
  
}

void AlexNet::fc8()
{
    for (int o = 0; o < 1000; o++)
    {
        double sum = fc8_bias[o];
        for (int i = 0; i < 4096; i++)
        {
            sum += fc7_out[i] * fc8_weight[o][i];
        }
        fc8_out[o] = sum;
    }
}

void AlexNet::softmax()
{
    double max_val = fc8_out[0];
    for (int i = 1; i < 1000; i++)
    {
        if (fc8_out[i] > max_val)
            max_val = fc8_out[i];
    }

    double sum_exp = 0.0;
    for (int i = 0; i < 1000; i++)
    {
        softmax_out[i] = exp(fc8_out[i] - max_val);
        sum_exp += softmax_out[i];
    }

    for (int i = 0; i < 1000; i++)
    {
        softmax_out[i] /= sum_exp;
    }
}

void AlexNet::run()
{
    if (rst.read() == 1)
    {
        out_valid.write(0);
        done = false;
        loaded = false;

        for (int i = 0; i < 1000; i++)
        {
            output_linear[i].write(0.0);
            output_softmax[i].write(0.0);
        }
    }
    else if (in_valid.read() == 1 && !done)
    {
        if (!loaded)
        {
           load_conv1_weight_bias();
           load_conv2_weight_bias();
           load_conv3_weight_bias();
           load_conv4_weight_bias();
           load_conv5_weight_bias();
           load_fc6_weight_bias();
           load_fc7_weight_bias();
           load_fc8_weight_bias(); 
           loaded = true;
        }

        load_input();
        conv1();
        relu1();
        pool1();
        conv2();
        relu2();
        pool2();
        conv3();
        relu3();
        conv4();
        relu4();
        conv5();
        relu5();
        pool5();
        flatten_pool5();
        fc6();    
        relu6();
        fc7();
        relu7();
        fc8();
        softmax(); 

        
        for (int i = 0; i < 1000; i++)
{
    output_linear[i].write(fc8_out[i]);
    output_softmax[i].write(softmax_out[i]);
}

out_valid.write(1);
done = true;

       
}

else
{
    out_valid.write(0);
}
}



