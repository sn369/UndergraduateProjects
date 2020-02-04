`timescale 1ns / 1ps
module InsMEM( 
	input InsMemRW, //指令存储器读写控制信号，�?0写，�?1�?
	input[31:0] IAddr, //指令存储器地�?输入端口
	output reg[31:0] IDataOut //指令代码输出端口
	);
	
	reg[7:0] ins[99:0];
	initial begin
		$readmemb ("C:/Users/x/Desktop/instruction.txt", ins);
	end

	//读指�?
	always@(InsMemRW or IAddr) begin
		if(InsMemRW) begin
			IDataOut[31:24] = ins[IAddr];
			IDataOut[23:16] = ins[IAddr+1];
			IDataOut[15:8] = ins[IAddr+2];
			IDataOut[7:0] = ins[IAddr+3];
		end
	end
endmodule