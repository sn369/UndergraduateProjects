`timescale 1ns / 1ps
module PC(
    input CLK, Reset, PCWre,
	input[31:0] nextPC, //��һ��ָ���ַ
    output reg[31:0] curPC //��ǰָ���ַ
    );
    
    always @(posedge CLK or negedge Reset) begin
        if (Reset==1) //ResetΪ0��pc��Ϊ0
            curPC = 0;
        else if (PCWre) //PCWreΪ1��pc�ı�
			curPC = nextPC;
        else if(!PCWre) //PCWreΪ0��pc���ı�
			curPC = curPC;
    end  
endmodule 