#ifndef __FC_LAYER_H__
#define __FC_LAYER_H__

#include "ap_int.h"
#include "hls_stream.h"
#include "config.h"

template <
    int I_CH,
    int O_CH
>
void fc_layer(
    hls::stream<NN_VAL> &stream_in,
    const ap_int<8>  WEIGHT[I_CH][O_CH],
    const ap_int<32>  BIAS[O_CH],
    const ap_int<32> REQUANT[O_CH],
    const ap_int<32> SHIFT[O_CH],
    hls::stream<NN_VAL> &stream_out,
    bool use_relu = true
){
    static NN_VAL IN[I_CH];

    #pragma HLS BIND_STORAGE variable=IN type=ram_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=WEIGHT type=rom_2p impl=bram
    #pragma HLS BIND_STORAGE variable=BIAS type=rom_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=REQUANT type=rom_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=SHIFT type=rom_2p impl=lutram

    #pragma HLS ARRAY_PARTITION variable=IN dim=1 complete
    #pragma HLS ARRAY_PARTITION variable=WEIGHT cyclic factor=2 dim=1
    #pragma HLS ARRAY_PARTITION variable=BIAS dim=1 complete
    #pragma HLS ARRAY_PARTITION variable=REQUANT dim=1 complete
    #pragma HLS ARRAY_PARTITION variable=SHIFT dim=1 complete

    for(int ich = 0; ich < I_CH; ich++){
#pragma HLS PIPELINE II=1
        IN[ich] = stream_in.read();
    }

    for(int och = 0; och < O_CH; och++){
#pragma HLS UNROLL factor=10
#pragma HLS loop_tripcount min = 1 max = O_CH avg = O_CH / 2
        ap_int<32> acc = 0;
        for(int ich = 0; ich < I_CH; ich++){
#pragma HLS PIPELINE II=1
            acc += IN[ich]*WEIGHT[ich][och];
        }

        acc+=BIAS[och];
        if(use_relu && acc < 0){
            acc = 0;
        }

        ap_int<32> requant = requantize(acc, REQUANT[och], SHIFT[och]);

        stream_out.write(sat_int8(requant));
    }
}

template <
    int I_CH
>
void argmax(
    hls::stream<NN_VAL> &stream_in,
    hls::stream<NN_VAL> &stream_out
){
    static NN_VAL IN[I_CH];
    #pragma HLS BIND_STORAGE variable=IN type=ram_2p impl=lutram

    #pragma HLS ARRAY_PARTITION variable=IN dim=1 complete

    for(int ich = 0; ich < I_CH; ich++){
#pragma HLS PIPELINE II=1
        IN[ich] = stream_in.read();
    }

    NN_VAL max = IN[0];
    NN_VAL max_idx = 0;
    for(int i = 0; i < I_CH; i++){
#pragma HLS UNROLL factor=2
        NN_VAL curr = IN[i];
        if(curr > max){
            max_idx = (ap_int<8>)i;
            max = curr;
        }
    }
    stream_out.write(max_idx);
}

#endif