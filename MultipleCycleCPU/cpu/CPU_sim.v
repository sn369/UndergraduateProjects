`timescale 1ns / 1ps
module cpu_sim();
    reg  CLK;
    reg  Reset;
    wire [31:0] curPC,nextPC,ReadData1,ReadData2,
	result,WriteData;
    wire [4:0] rs,rt;
    MultiCycleCPU cpu(
    .CLK(CLK),
    .Reset(Reset),
    .curPC(curPC),
	.nextPC(nextPC),
	.ReadData1(ReadData1),
	.ReadData2(ReadData2),
	.result(result),
	.WriteData(WriteData),
	.rs(rs),
	.rt(rt)
	);
    initial begin
        Reset = 1;
        CLK = 0;
        #15 Reset=0;
    end
	always #10 CLK = ~CLK;
endmodule