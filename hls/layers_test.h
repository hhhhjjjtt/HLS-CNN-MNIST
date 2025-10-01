#ifndef __LAYERS_TEST_H__
#define __LAYERS_TEST_H__

#include "config.h"

#include <iostream>

void layers_test(hls::stream<axi_packet_t>& stream_in,
                 hls::stream<axi_packet_t>& stream_out);
            
#endif