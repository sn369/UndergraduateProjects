`timescale 1ns / 1ps
module muxA(
	input ALUSrcA,
	input[31:0] ReadData1,
	input[4:0] sa,
	output reg[31:0] A
	);
	always@(*) begin
		if(ALUSrcA) begin
			A[31:5]=27'h0000000;
			A[4:0]=sa;
		end
		else A=ReadData1;
	end
endmodule