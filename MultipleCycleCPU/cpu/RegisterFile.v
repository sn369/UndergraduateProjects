`timescale 1ns / 1ps
module RegFile(
	input CLK,
	input RegWre, //写使能信号
	input [4:0] rs,rt,WriteReg,
	input [31:0] WriteData,
	output [31:0] ReadData1, ReadData2
	);
	reg [31:0] register[0:31]; //定义32个32位的寄存器
	
	//读寄存器
	initial begin
	   register[0]=0; //初始化0号寄存器
	end;
	assign ReadData1=register[rs];
	assign ReadData2=register[rt];
	
	always @ (negedge CLK) begin // 必须用时钟边沿触发
		if(RegWre==1 && WriteReg!=0)
			register[WriteReg]<=WriteData;
	end
endmodule