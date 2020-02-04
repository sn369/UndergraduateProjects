`timescale 1ns / 1ps

module InsRegister(
	input CLK, IRWre,
	input [31:0] IDataOut,
	output reg[31:0] Ins,
	output reg[5:0] opCode,
	output reg[4:0] rs,rt,rd,sa,
	output reg[15:0] immediate,
	output reg[25:0] addr
);
	always @(negedge CLK) begin
		if(IRWre) begin
			Ins=IDataOut;
			opCode=Ins[31:26];
			rs=Ins[25:21];
			rt=Ins[20:16];
			rd=Ins[15:11];
			sa=Ins[10:6];
			immediate=Ins[15:0];
			addr=Ins[25:0];
		end
	end
endmodule