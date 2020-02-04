module mux2to1_32bit(
	input select,
	input [31:0]DataA,DataB,
	output reg[31:0] trueData
	);
	
	always@(*) begin
		trueData=select?DataA:DataB;
	end
endmodule