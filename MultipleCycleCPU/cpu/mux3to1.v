`timescale 1ns / 1ps
module mux3to1(
	input [1:0] RegDst,
	input [4:0] rd, rt,
	output reg[4:0] WriteReg
);
	always@(*) begin
		case(RegDst)
		 2'b00: WriteReg=5'b11111;
		 2'b01: WriteReg=rt;
		 2'b10: WriteReg=rd;
		endcase
	end
endmodule