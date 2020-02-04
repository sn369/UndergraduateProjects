`timescale 1ns / 1ps
module display(CLK, Reset, swOp, key_in, an, dispcode);
	input CLK;
	input Reset;
	input [1:0] swOp;
	input key_in;
	output [3:0] an;
	output [6:0] dispcode;
	
	wire key_clk;
	debounce debounce(CLK, key_in, key_clk);
    
    wire [31:0] curPC,nextPC,ReadData1_out,ReadData2_out,alu_out,WriteData;
    wire [4:0] rs,rt;
    wire n_Rst;
    assign n_Rst = ~Reset;
    MultiCycleCPU cpu(key_clk,n_Rst,curPC,nextPC,ReadData1_out,
	ReadData2_out,alu_out,WriteData,rs,rt);
	
	num_to_dispcode num_to_dispcode(CLK, Reset, swOp, curPC[7:0], nextPC[7:0], rs, 
									ReadData1_out[7:0], rt, ReadData2_out[7:0], alu_out[7:0], 
									WriteData[7:0], dispcode, an);
	
endmodule
