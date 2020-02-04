`timescale 1ns / 1ps
module ALU(
	input[31:0] A, B,
	input[2:0] ALUOp,
	output zero, sign,
	output reg[31:0] result
	);
	
	assign zero = (result)?0:1;
	assign sign = (result[31])?1:0;
	always @(*) begin
	  case(ALUOp)  //各类运算
		3'b000: result=A+B;
		3'b001: result=A-B;
		3'b010: result=(A<B)?1:0;
		3'b011: begin
				if (A<B&&(A[31]==B[31])) 
				result=1;
				else if (A[31]&&!B[31]) result=1;
				else result=0;
				end
		3'b100: result=B<<A;
		3'b101: result=A|B;
		3'b110: result=A&B;
		3'b111: result=(~A & B) | (A & ~B);
		endcase
	end
endmodule