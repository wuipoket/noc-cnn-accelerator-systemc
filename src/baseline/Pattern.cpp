#include "Pattern.h"
#include "ROM.h"
#include "MemoryMap.h"
#include "ComputeModel.h"
bool compare(const pair<double, int>& a, const pair<double, int>& b)
{
    return a.first > b.first;
}

int Pattern::get_layer_id(int s)
{
    if (s == 0) return 0;
    if (s == 1 || s == 2) return 1;
    if (s == 3 || s == 4) return 2;
    if (s == 5 || s == 6) return 3;
    if (s == 7 || s == 8) return 4;
    if (s == 9 || s == 10) return 5;
    if (s == 11 || s == 12) return 6;
    if (s == 13 || s == 14) return 7;
    if (s == 15 || s == 16) return 8;
    return 0;
}

bool Pattern::get_layer_type(int s)
{
    if (s == 0) return false;
    return (s % 2 == 0);
}

int Pattern::get_stage_count(int s)
{
    switch (s)
    {
        case 0:  return 3 * 224 * 224;

        case 1:  return 64 * 3 * 11 * 11;
        case 2:  return 64;

        case 3:  return 192 * 64 * 5 * 5;
        case 4:  return 192;

        case 5:  return 384 * 192 * 3 * 3;
        case 6:  return 384;

        case 7:  return 256 * 384 * 3 * 3;
        case 8:  return 256;

        case 9:  return 256 * 256 * 3 * 3;
        case 10: return 256;

        case 11: return 4096 * 9216;
        case 12: return 4096;

        case 13: return 4096 * 4096;
        case 14: return 4096;

        case 15: return 1000 * 4096;
        case 16: return 1000;

        default: return 0;
    }
}

void Pattern::store_stage_data(int s, int idx, double value)
{
    if (s == 0)
    {
        img[idx].write(value);
    }
    else if (s == 1)
    {
        int kw = idx % 11;
        int kh = (idx / 11) % 11;
        int ic = (idx / (11 * 11)) % 3;
        int oc = idx / (3 * 11 * 11);
        net->conv1_weight[oc][ic][kh][kw] = value;
    }
    else if (s == 2)
    {
        net->conv1_bias[idx] = value;
    }
    else if (s == 3)
    {
        int kw = idx % 5;
        int kh = (idx / 5) % 5;
        int ic = (idx / (5 * 5)) % 64;
        int oc = idx / (64 * 5 * 5);
        net->conv2_weight[oc][ic][kh][kw] = value;
    }
    else if (s == 4)
    {
        net->conv2_bias[idx] = value;
    }
    else if (s == 5)
    {
        int kw = idx % 3;
        int kh = (idx / 3) % 3;
        int ic = (idx / (3 * 3)) % 192;
        int oc = idx / (192 * 3 * 3);
        net->conv3_weight[oc][ic][kh][kw] = value;
    }
    else if (s == 6)
    {
        net->conv3_bias[idx] = value;
    }
    else if (s == 7)
    {
        int kw = idx % 3;
        int kh = (idx / 3) % 3;
        int ic = (idx / (3 * 3)) % 384;
        int oc = idx / (384 * 3 * 3);
        net->conv4_weight[oc][ic][kh][kw] = value;
    }
    else if (s == 8)
    {
        net->conv4_bias[idx] = value;
    }
    else if (s == 9)
    {
        int kw = idx % 3;
        int kh = (idx / 3) % 3;
        int ic = (idx / (3 * 3)) % 256;
        int oc = idx / (256 * 3 * 3);
        net->conv5_weight[oc][ic][kh][kw] = value;
    }
    else if (s == 10)
    {
        net->conv5_bias[idx] = value;
    }
    else if (s == 11)
    {
        int i = idx % 9216;
        int o = idx / 9216;
        net->fc6_weight[o][i] = value;
    }
    else if (s == 12)
    {
        net->fc6_bias[idx] = value;
    }
    else if (s == 13)
    {
        int i = idx % 4096;
        int o = idx / 4096;
        net->fc7_weight[o][i] = value;
    }
    else if (s == 14)
    {
        net->fc7_bias[idx] = value;
    }
    else if (s == 15)
    {
        int i = idx % 4096;
        int o = idx / 4096;
        net->fc8_weight[o][i] = value;
    }
    else if (s == 16)
    {
        net->fc8_bias[idx] = value;
    }
}

void Pattern::run()
{
    if (cycle == 0)
    {
        rst.write(1);
        in_valid.write(0);
        layer_id_valid.write(false);
    }
    else if (cycle == 1)
    {
        rst.write(0);
        in_valid.write(0);
        layer_id_valid.write(false);
    }
    else if (stage < 17)
    {
        rst.write(0);
        in_valid.write(0);

        if (load_state == 0)
        {
            elem_idx = 0;
            layer_id.write(get_layer_id(stage));
            layer_id_type.write(get_layer_type(stage));
            layer_id_valid.write(true);
            load_state = 1;
        }
        else if (load_state == 1)
        {
            layer_id_valid.write(false);
            load_state = 2;
        }
        else if (load_state == 2)
        {
            layer_id_valid.write(false);

            if (rom_data_valid.read())
            {
                if (elem_idx < get_stage_count(stage))
                {
                    store_stage_data(stage, elem_idx, (double)rom_data.read());
                    elem_idx++;
                }

                if (elem_idx == get_stage_count(stage))
                {
                    load_state = 3;
                }
            }
        }
        else if (load_state == 3)
        {
            layer_id_valid.write(false);

            if (!rom_data_valid.read())
            {
                stage++;
                load_state = 0;
                elem_idx = 0;
            }
        }
    }
    else if (out_valid.read() == 1)
    {
        ifstream class_file("data/imagenet_classes.txt");
        vector<string> class_name;
        string class_name_element;

        while (getline(class_file, class_name_element))
        {
            class_name.push_back(class_name_element);
        }

        vector<float> linear_buffer(1000);
	vector<float> softmax_buffer(1000);
	vector<float> linear_from_dram(1000);
	vector<float> softmax_from_dram(1000);

	for (int i = 0; i < 1000; ++i)
	{	
    	    linear_buffer[i] = (float)output_linear[i].read();
            softmax_buffer[i] = (float)output_softmax[i].read();
        }	

	if (g_dma == nullptr) {
           cout << "Error: AXI DMA is not available for output write-back." << endl;
           exit(1);
        }   

	g_dma->dma_write(DRAM_OUTPUT_BASE, linear_buffer.data(), 1000);
	g_dma->dma_write(DRAM_OUTPUT_BASE + 1000 * 4, softmax_buffer.data(), 1000);

	g_dma->dma_read(DRAM_OUTPUT_BASE, linear_from_dram.data(), 1000);
	g_dma->dma_read(DRAM_OUTPUT_BASE + 1000 * 4, softmax_from_dram.data(), 1000);

	vector<pair<double, int> > indexed_values;

	for (int i = 0; i < 1000; ++i)
	{
            indexed_values.push_back(make_pair((double)softmax_from_dram[i], i));
        } 

	sort(indexed_values.begin(), indexed_values.end(), compare);

        cout << "Top 100 classes:" << endl;
        cout << "=================================================" << endl;
        cout << fixed << setprecision(3);
        cout << right << setw(5) << "idx"
             << " | " << setw(8) << "val"
             << " | " << setw(11) << "possibility"
             << " | " << "class name" << endl;
        cout << "-------------------------------------------------" << endl;

        for (int i = 0; i < 100; ++i)
        {
            cout << right << setw(5) << indexed_values[i].second
                 << " | " << setw(8) << double(linear_from_dram[indexed_values[i].second]) 
                 << " | " << setw(11) << (indexed_values[i].first * 100)
                 << " | " << class_name[indexed_values[i].second] << endl;
        }

        cout << "=================================================" << endl;
        g_dma->print_stats();
        ComputeModel::print_stats(); 
        exit(0);
    }
    else
    {
        layer_id_valid.write(false);

        if (!network_started)
        {
            net->loaded = true;
            network_started = true;
        }

        in_valid.write(1);
    }

    cycle++;

    if (cycle == CYCLE)
    {
        exit(0);
    }
}
