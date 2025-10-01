
wire kernel_monitor_reset;
wire kernel_monitor_clock;
wire kernel_monitor_report;
assign kernel_monitor_reset = ~ap_rst_n;
assign kernel_monitor_clock = ap_clk;
assign kernel_monitor_report = 1'b0;
wire [1:0] axis_block_sigs;
wire [11:0] inst_idle_sigs;
wire [8:0] inst_block_sigs;
wire kernel_block;

assign axis_block_sigs[0] = ~axis_to_nn_784_U0.stream_in_TDATA_blk_n;
assign axis_block_sigs[1] = ~nn_to_axis_1_U0.stream_out_TDATA_blk_n;

assign inst_idle_sigs[0] = axis_to_nn_784_U0.ap_idle;
assign inst_block_sigs[0] = (axis_to_nn_784_U0.ap_done & ~axis_to_nn_784_U0.ap_continue) | ~axis_to_nn_784_U0.input_to_conv0_blk_n;
assign inst_idle_sigs[1] = conv_layer_1_28_3_1_8_U0.ap_idle;
assign inst_block_sigs[1] = (conv_layer_1_28_3_1_8_U0.ap_done & ~conv_layer_1_28_3_1_8_U0.ap_continue) | ~conv_layer_1_28_3_1_8_U0.grp_conv_layer_1_28_3_1_8_Pipeline_VITIS_LOOP_34_2_VITIS_LOOP_35_3_fu_74.input_to_conv0_blk_n | ~conv_layer_1_28_3_1_8_U0.grp_conv_layer_Pipeline_VITIS_LOOP_46_4_VITIS_LOOP_47_5_VITIS_LOOP_48_6_fu_98.conv0_to_pool0_blk_n;
assign inst_idle_sigs[2] = pool_layer_8_28_2_2_U0.ap_idle;
assign inst_block_sigs[2] = (pool_layer_8_28_2_2_U0.ap_done & ~pool_layer_8_28_2_2_U0.ap_continue) | ~pool_layer_8_28_2_2_U0.grp_pool_layer_8_28_2_2_Pipeline_VITIS_LOOP_23_1_VITIS_LOOP_24_2_VITIS_LOOP_25_3_fu_30.conv0_to_pool0_blk_n | ~pool_layer_8_28_2_2_U0.grp_pool_layer_Pipeline_VITIS_LOOP_35_4_VITIS_LOOP_36_5_VITIS_LOOP_37_6_VITIS_LOOP_4_fu_38.pool0_to_conv1_blk_n;
assign inst_idle_sigs[3] = conv_layer_8_14_3_1_32_U0.ap_idle;
assign inst_block_sigs[3] = (conv_layer_8_14_3_1_32_U0.ap_done & ~conv_layer_8_14_3_1_32_U0.ap_continue) | ~conv_layer_8_14_3_1_32_U0.grp_conv_layer_Pipeline_VITIS_LOOP_33_1_VITIS_LOOP_34_2_VITIS_LOOP_35_3_fu_74.pool0_to_conv1_blk_n | ~conv_layer_8_14_3_1_32_U0.grp_conv_layer_Pipeline_VITIS_LOOP_46_4_VITIS_LOOP_47_5_VITIS_LOOP_48_6_VITIS_LOOP_5_fu_98.conv1_to_pool1_blk_n;
assign inst_idle_sigs[4] = pool_layer_32_14_2_2_U0.ap_idle;
assign inst_block_sigs[4] = (pool_layer_32_14_2_2_U0.ap_done & ~pool_layer_32_14_2_2_U0.ap_continue) | ~pool_layer_32_14_2_2_U0.grp_pool_layer_Pipeline_VITIS_LOOP_23_1_VITIS_LOOP_24_2_VITIS_LOOP_25_3_fu_30.conv1_to_pool1_blk_n | ~pool_layer_32_14_2_2_U0.grp_pool_layer_Pipeline_VITIS_LOOP_35_4_VITIS_LOOP_36_5_VITIS_LOOP_37_6_VITIS_LOOP_4_2_fu_38.pool1_to_pool2_blk_n;
assign inst_idle_sigs[5] = pool_layer_32_7_2_2_U0.ap_idle;
assign inst_block_sigs[5] = (pool_layer_32_7_2_2_U0.ap_done & ~pool_layer_32_7_2_2_U0.ap_continue) | ~pool_layer_32_7_2_2_U0.grp_pool_layer_32_7_2_2_Pipeline_VITIS_LOOP_23_1_VITIS_LOOP_24_2_VITIS_LOOP_25_3_fu_30.pool1_to_pool2_blk_n | ~pool_layer_32_7_2_2_U0.grp_pool_layer_Pipeline_VITIS_LOOP_35_4_VITIS_LOOP_36_5_VITIS_LOOP_37_6_VITIS_LOOP_4_1_fu_38.pool2_to_fc0_blk_n;
assign inst_idle_sigs[6] = fc_layer_288_10_U0.ap_idle;
assign inst_block_sigs[6] = (fc_layer_288_10_U0.ap_done & ~fc_layer_288_10_U0.ap_continue) | ~fc_layer_288_10_U0.grp_fc_layer_288_10_Pipeline_VITIS_LOOP_35_1_fu_765.pool2_to_fc0_blk_n | ~fc_layer_288_10_U0.fc0_to_argmax0_blk_n;
assign inst_idle_sigs[7] = argmax_10_U0.ap_idle;
assign inst_block_sigs[7] = (argmax_10_U0.ap_done & ~argmax_10_U0.ap_continue) | ~argmax_10_U0.grp_argmax_10_Pipeline_VITIS_LOOP_72_1_fu_55.fc0_to_argmax0_blk_n | ~argmax_10_U0.argmax0_to_output_blk_n;
assign inst_idle_sigs[8] = nn_to_axis_1_U0.ap_idle;
assign inst_block_sigs[8] = (nn_to_axis_1_U0.ap_done & ~nn_to_axis_1_U0.ap_continue) | ~nn_to_axis_1_U0.argmax0_to_output_blk_n;

assign inst_idle_sigs[9] = 1'b0;
assign inst_idle_sigs[10] = axis_to_nn_784_U0.ap_idle;
assign inst_idle_sigs[11] = nn_to_axis_1_U0.ap_idle;

layers_test_hls_deadlock_idx0_monitor layers_test_hls_deadlock_idx0_monitor_U (
    .clock(kernel_monitor_clock),
    .reset(kernel_monitor_reset),
    .axis_block_sigs(axis_block_sigs),
    .inst_idle_sigs(inst_idle_sigs),
    .inst_block_sigs(inst_block_sigs),
    .block(kernel_block)
);


always @ (kernel_block or kernel_monitor_reset) begin
    if (kernel_block == 1'b1 && kernel_monitor_reset == 1'b0) begin
        find_kernel_block = 1'b1;
    end
    else begin
        find_kernel_block = 1'b0;
    end
end
