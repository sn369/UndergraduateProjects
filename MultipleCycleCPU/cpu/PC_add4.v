`timescale 1ns / 1ps
module PC_add4(
	input[31:0] curPC, //当前指令地址
    output[31:0] PC4 //PC4
    );
	
	assign PC4=curPC+4;
endmodule