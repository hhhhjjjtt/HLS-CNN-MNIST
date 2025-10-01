#ifndef __POOL_LAYER_H__
#define __POOL_LAYER_H__

#include "ap_int.h"
#include "hls_stream.h"
#include "config.h"

template <
    int I_CH,
    int I_DIM,
    int KERNEL_SIZE,  
    int STRIDE
>
void pool_layer(
    hls::stream<NN_VAL> &stream_in,
    hls::stream<NN_VAL> &stream_out,    
    bool MODE = 0       //0 -> Max; 1 -> Average
){
    static NN_VAL IN[I_CH][I_DIM][I_DIM];

    #pragma HLS BIND_STORAGE variable=IN type=ram_2p impl=lutram

    for(int ich = 0; ich < I_CH; ich++){
        for(int irow = 0; irow < I_DIM; irow++){
            for(int icol = 0; icol < I_DIM; icol++){
#pragma HLS PIPELINE II=1
                IN[ich][irow][icol] = stream_in.read();
            }
        }
    }

    int O_CH = I_CH; 
    int O_DIM = (I_DIM-KERNEL_SIZE)/STRIDE + 1; 

    for(int och = 0; och < O_CH; och++){
        for(int orow = 0; orow < O_DIM; orow++){
            for(int ocol = 0; ocol < O_DIM; ocol++){
                ap_int<8> max = IN[och][orow * STRIDE][ocol * STRIDE];
                ap_int<16> acc = 0;
                for(int k_row = 0; k_row < KERNEL_SIZE; k_row++){
                    for(int k_col = 0; k_col < KERNEL_SIZE; k_col++){
#pragma HLS PIPELINE II=1
                        int irow = orow*STRIDE + k_row;
                        int icol = ocol*STRIDE + k_col;
                        
                        if(max < IN[och][irow][icol]){
                            max = IN[och][irow][icol];
                        }
                        acc += IN[och][irow][icol];
                    }
                }
                stream_out.write((MODE == 0) ? max : (ap_int<8>)(acc/(KERNEL_SIZE*KERNEL_SIZE)));
            }
        }
    }
}

#endif