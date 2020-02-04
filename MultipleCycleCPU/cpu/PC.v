`timescale 1ns / 1ps
module PC(
    input CLK, Reset, PCWre,
	input[31:0] nextPC, //下一条指令地址
    output reg[31:0] curPC //当前指令地址
    );
    
    always @(posedge CLK or negedge Reset) begin
        if (Reset==1) //Reset为0，pc置为0
            curPC = 0;
        else if (PCWre) //PCWre为1，pc改变
			curPC = nextPC;
        else if(!PCWre) //PCWre为0，pc不改变
			curPC = curPC;
    end  
endmodule 