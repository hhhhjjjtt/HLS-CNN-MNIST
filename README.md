# HLS-CNN-MNIST
Integer-only quantized CNN for MNIST built in **Vitis HLS** and deployed on **PYNQ-Z2** with a test accuracy of 98.03%

## Quick Start: Inference on PYNQ-Z2
Copy the `run-on-pynqz2` to your pynq board(Jupyter notebooks folder).

Then, run the script in `conv_test06.ipynb`

## Notes
**Vitis HLS**: See hls/ if you want to rebuild or modify the IP.

## Model Formats
**Input**: a stream of 784 `int8` numbers

**Weights**: int8 

**Biases**: int32

**Requantization**: after each layer’s int32 multiply-and accumulate, we rescale with a **per-output-channel** integer multiplier **M** and right-shift **S**, then clamp to int8 to keep activations in range:

$$
y_{\text{int8}} = \text{clip}\left(\frac{\text{acc}_{\text{int32}}\cdot M_o \;\pm\; 2^{S-1}}{2^{S}},\,-128,\,127\right)
$$

We pick $$S = 30$$ 
and 
$$M_o \;=\; \left\lfloor \frac{s_x\, s_{w,o}}{s_y} \, 2^{S} \;+\; \tfrac{1}{2} \right\rfloor$$

Where:
- $s_x$ is the input activation scale
- $s_{w,o}$ is the weight scale for output channel
- $s_y$ is the output activation scale

**Output**: a int8 class index from 0-9.