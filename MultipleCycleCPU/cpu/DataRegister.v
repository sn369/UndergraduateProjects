`timescale 1ns / 1ps

module DataRegister(
	input CLK,
	input [31:0] inData,
	output reg [31:0] outData
);

	always@(negedge CLK) begin
		outData=inData;
	end
endmodule