`timescale 1ns / 1ps
module num_to_dispcode(
	input wire CLK, 
	input wire Reset,
	input wire [1:0] swOp,
	
	input wire [7:0] curAddr_l8b,
	input wire [7:0] nextAddr_l8b,
    input wire [4:0] rs,
	input wire [7:0] rsData_l8b,
    input wire [4:0] rt,
	input wire [7:0] rtData_l8b,
    input wire [7:0] result_l8b,
	input wire [7:0] writeData_l8b,
	
	output reg [6:0] dispcode, 
	output reg [3:0] an
	);
	
	//----------↓ 扫描时钟分频 
	wire [1:0] s;			 //位选值 
	reg [19:0] clkdiv;		//分频计数数组 
	assign s = clkdiv[19:18];	//完成分频 
	always @(posedge CLK)
		clkdiv <= clkdiv + 1;
	//----------↑ 
	
	reg [3:0] display_data; //待译码数 
	
	//----------↓ 功能选以及位选 
	always@(posedge CLK) begin
		if(!Reset) an=4'b1111;
		else
		case(s)
			0 : begin 
					an=4'b1110;
					case(swOp) 
						0 : display_data = nextAddr_l8b[3:0];
						1 : display_data = rsData_l8b[3:0];
						2 : display_data = rtData_l8b[3:0];
						3 : display_data = writeData_l8b[3:0];
					endcase
				end
			1 : begin 
					an=4'b1101;
					case(swOp) 
						0 : display_data = nextAddr_l8b[7:4];
						1 : display_data = rsData_l8b[7:4];
						2 : display_data = rtData_l8b[7:4];
						3 : display_data = writeData_l8b[7:4];
					endcase
				end
			2 : begin 
					an=4'b1011;
					case(swOp) 
						0 : display_data = curAddr_l8b[3:0];
						1 : display_data = rs[3:0];
						2 : display_data = rt[3:0];
						3 : display_data = result_l8b[3:0];
					endcase
				end
			3 : begin 
					an=4'b0111;
					case(swOp) 
						0 : display_data = curAddr_l8b[7:4];
						1 : display_data = {3'b000,rs[4]};
						2 : display_data = {3'b000,rt[4]};
						3 : display_data = result_l8b[7:4];
					endcase
				end
		endcase
	end
	//----------↑ 
	
	//----------↓译码 
	always @(posedge CLK)   
	   case (display_data)         
                4'b0000 : dispcode = 7'b0000001;  //0；'0'-亮灯，'1'-熄灯         
                4'b0001 : dispcode = 7'b1001111;  //1         
                4'b0010 : dispcode = 7'b0010010;  //2         
                4'b0011 : dispcode = 7'b0000110;  //3         
                4'b0100 : dispcode = 7'b1001100;  //4         
                4'b0101 : dispcode = 7'b0100100;  //5         
                4'b0110 : dispcode = 7'b0100000;  //6          
                4'b0111 : dispcode = 7'b0001111;  //7         
                4'b1000 : dispcode = 7'b0000000;  //8         
                4'b1001 : dispcode = 7'b0000100;  //9         
                4'b1010 : dispcode = 7'b0001000;  //A         
                4'b1011 : dispcode = 7'b1100000;  //b         
                4'b1100 : dispcode = 7'b0110001;  //C         
                4'b1101 : dispcode = 7'b1000010;  //d         
                4'b1110 : dispcode = 7'b0110000;  //E         
                4'b1111 : dispcode = 7'b0111000;  //F         
                default : dispcode = 7'b1111110;  //不亮     
	    endcase  
	//-----------↑ 
endmodule