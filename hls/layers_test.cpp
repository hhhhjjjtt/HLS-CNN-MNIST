#include "layers_test.h"
#include "conv_layer.h"
#include "fc_layer.h"
#include "pool_layer.h"
#include "weights.h"

void layers_test(
    hls::stream<axi_packet_t>& stream_in,
    hls::stream<axi_packet_t>& stream_out
){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis port=stream_in
#pragma HLS INTERFACE axis port=stream_out
#pragma HLS DATAFLOW

    hls::stream<NN_VAL> input_to_conv0, conv0_to_pool0, pool0_to_conv1, conv1_to_pool1, pool1_to_pool2, pool2_to_fc0, fc0_to_argmax0, argmax0_to_output;

#pragma HLS STREAM variable=input_to_conv0      depth=1000
#pragma HLS STREAM variable=conv0_to_pool0      depth=10000
#pragma HLS STREAM variable=pool0_to_conv1      depth=2000
#pragma HLS STREAM variable=conv1_to_pool1      depth=10000
#pragma HLS STREAM variable=pool1_to_pool2      depth=2000
#pragma HLS STREAM variable=pool2_to_fc0        depth=3000
#pragma HLS STREAM variable=fc0_to_argmax0      depth=100
#pragma HLS STREAM variable=argmax0_to_output   depth=10

    axis_to_nn<784>(stream_in, input_to_conv0);
    
    conv_layer<1, 28, 3, 1, 8>(input_to_conv0, FILTER_conv0, BIAS_conv0, REQUANT_conv0, SHIFT_conv0, conv0_to_pool0);

    pool_layer<8, 28, 2, 2>(conv0_to_pool0, pool0_to_conv1, 0);

    conv_layer<8, 14, 3, 1, 32>(pool0_to_conv1, FILTER_conv1, BIAS_conv1, REQUANT_conv1, SHIFT_conv1, conv1_to_pool1);

    pool_layer<32, 14, 2, 2>(conv1_to_pool1, pool1_to_pool2, 0);

    pool_layer<32, 7, 2, 2>(pool1_to_pool2, pool2_to_fc0, 1);

    fc_layer<288, 10>(pool2_to_fc0, WEIGHT_fc0, BIAS_fc0, REQUANT_fc0, SHIFT_fc0, fc0_to_argmax0, false);

    argmax<10>(fc0_to_argmax0, argmax0_to_output);

    nn_to_axis<1>(argmax0_to_output, stream_out);
}