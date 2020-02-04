`timescale 1ns / 1ps
module DataMEM(
	input CLK,
	input RD,  //Îª0£¬¶Á
	input WR,  //Îª0£¬Ð´
	input[31:0] Daddr,
	input[31:0] DataIn,
	output[31:0] DataOut
	);
	reg[7:0] ram[0:60];
	
	//¶Á
	assign DataOut[31:24]=(RD)? 8'bz:ram[Daddr];
	assign DataOut[23:16]=(RD)? 8'bz:ram[Daddr+1];
	assign DataOut[15:8]=(RD)? 8'bz:ram[Daddr+2];
	assign DataOut[7:0]=(RD)? 8'bz:ram[Daddr+3];
	//Ð´
	always@(negedge CLK) begin
		if(!WR) begin
			ram[Daddr]<=DataIn[31:24];
			ram[Daddr+1]<=DataIn[23:16];
			ram[Daddr+2]<=DataIn[15:8];
			ram[Daddr+3]<=DataIn[7:0];
		end
	end
endmodule