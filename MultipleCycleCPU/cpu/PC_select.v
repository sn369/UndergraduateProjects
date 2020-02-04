`timescale 1ns / 1ps
module PC_select(
	input[31:0] curPC, PC4,
	input[1:0] PCSrc,
	input[31:0] ExtImme, ReadData1,
	input[25:0] addr,
	output reg[31:0] nextPC 
    );
    always@(*) begin
        case(PCSrc)
            2'b00 : nextPC = PC4;
            2'b01 : nextPC = PC4+(ExtImme<<2); //beq,bne,bgtz
            2'b10 : nextPC = ReadData1; //jr
			2'b11 : begin //j,jal
					nextPC = PC4;
				    nextPC[27:0] = addr<<2;
			end
        endcase
	end
endmodule 