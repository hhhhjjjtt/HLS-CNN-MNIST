#ifndef __CONV_LAYER_H__
#define __CONV_LAYER_H__

#include "ap_int.h"
#include "hls_stream.h"
#include "config.h"

template <
    int I_CH,
    int I_DIM,
    int KERNEL_SIZE,
    int STRIDE,
    int O_CH
>
void conv_layer(
    hls::stream<NN_VAL> &stream_in,        //[I_CH][I_ROW][I_COL]
    const ap_int<8>   FILTER[O_CH][I_CH][KERNEL_SIZE][KERNEL_SIZE],
    const ap_int<32>  BIAS[O_CH],
    const ap_int<32> REQUANT[O_CH],
    const ap_int<32> SHIFT[O_CH],
    hls::stream<NN_VAL> &stream_out,       //[O_CH][O_ROW][O_COL]
    bool use_relu = true
){
    static NN_VAL IN[I_CH][I_DIM][I_DIM];

    #pragma HLS BIND_STORAGE variable=IN type=ram_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=FILTER type=rom_2p impl=bram
    #pragma HLS BIND_STORAGE variable=BIAS type=rom_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=REQUANT type=rom_2p impl=lutram
    #pragma HLS BIND_STORAGE variable=SHIFT type=rom_2p impl=lutram
    
    #pragma HLS LOOP_TRIPCOUNT avg=64 max=127 min=1
    for(int ich = 0; ich < I_CH; ich++){
        for(int irow = 0; irow < I_DIM; irow++){
            for(int icol = 0; icol < I_DIM; icol++){
#pragma HLS PIPELINE II=1
                IN[ich][irow][icol] = stream_in.read();
            }
        }
    }


    const int PAD = KERNEL_SIZE/2; 
    const int O_DIM = (I_DIM + 2*PAD - KERNEL_SIZE)/STRIDE + 1;

    for(int och = 0; och < O_CH; och++){
        for(int orow = 0; orow < O_DIM; orow++){
            for(int ocol = 0; ocol < O_DIM; ocol++){
                ap_int<32> acc = 0;
                for(int ich = 0; ich < I_CH; ich++){
#pragma HLS PIPELINE II=1
                    for(int krow = 0; krow < KERNEL_SIZE; krow++){
                        for(int kcol = 0; kcol < KERNEL_SIZE; kcol++){
                            int irow = orow*STRIDE - PAD + krow;
                            int icol = ocol*STRIDE - PAD + kcol;
                            bool in_padding = (irow < 0) || (irow > I_DIM-1) || (icol < 0) || (icol > I_DIM-1);
                            ap_int<8> ival = in_padding ? (ap_int<8>)0 : IN[ich][irow][icol];
                            acc += (ap_int<32>)ival * (ap_int<32>)FILTER[och][ich][krow][kcol];
                        }
                    }    
                }
                
                acc += BIAS[och]; // at position orow, ocol of output matrix
                if(use_relu && acc < 0){
                    acc = 0;
                }

                ap_int<32> requant = requantize(acc, REQUANT[och], SHIFT[och]);

                stream_out.write(sat_int8(requant));
            }
        }
    }
}

#endif