`timescale 1ns / 1ps
module SignZeroExtend(
	input[15:0] Immediate, 
	input ExtSel,
	output[31:0] ExtImme
	);
	assign ExtImme[15:0]=Immediate;
	assign ExtImme[31:16]=ExtSel?(Immediate[15]?16'hffff:16'h0000):16'h0000;
endmodule