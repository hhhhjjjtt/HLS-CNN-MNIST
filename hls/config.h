#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

typedef ap_int<8> NN_VAL;
typedef ap_axis<8, 0, 0, 0> axi_packet_t;

static inline ap_int<8> sat_int8(ap_int<32> v){
    if (v > 127)  return (ap_int<8>)127;
    if (v < -128) return (ap_int<8>)-128;
    return (ap_int<8>)v;
}

static inline ap_int<32> requantize(ap_int<32> acc, ap_int<32> M, ap_int<32> S) {
#pragma HLS inline
    ap_uint<6> s = (S <= 0) ? (ap_uint<6>)0 : (S > 31 ? (ap_uint<6>)31 : (ap_uint<6>)S);

    ap_int<64> prod = (ap_int<64>)acc * (ap_int<64>)M;
    if (s == 0) {
        return (ap_int<32>)prod;
    }
    ap_int<64> round = (prod >= 0) ? ((ap_int<64>)1 << (s - 1))
                                   : (ap_int<64>)-(1 << (s - 1));
    ap_int<64> shifted = (prod + round) >> s;
    return (ap_int<32>)shifted;
}

template<int N>
void axis_to_nn(hls::stream<axi_packet_t>& axi_in,
                hls::stream<NN_VAL>& nn_out) {
  for (int i = 0; i < N; ++i) {
  #pragma HLS PIPELINE II=1
    axi_packet_t p = axi_in.read();
    nn_out.write((NN_VAL)p.data);
  }
}

template<int N>
void nn_to_axis(hls::stream<NN_VAL>& nn_in,
                hls::stream<axi_packet_t>& axi_out) {
  for (int i = 0; i < N; ++i) {
  #pragma HLS PIPELINE II=1
    axi_packet_t p;
    p.data = nn_in.read();
    p.keep = 1;   // one byte valid
    p.strb = 1;   // one byte strobe
    p.last = (i == N - 1);
    axi_out.write(p);
  }
}

#endif