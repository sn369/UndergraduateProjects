`timescale 1ns / 1ps  
  
module MultiCycleCPU(  
    input CLK, Reset,
    output wire [31:0] curPC,nextPC,ReadData1,ReadData2,
	result,WriteData,
	output wire [4:0] rs,rt
    );
      
	wire [1:0] RegDst;
    wire [15:0] immediate;
	wire [25:0] addr;
	wire [31:0] PC4,DB,Ins,IDataOut,ExtImme,IAddr,Daddr,DataOut,ReadData1_out,ReadData2_out;
	wire [31:0] A,B,alu_out,DB_out;
	wire  zero,sign,WrRegDSrc,InsMemRW,RD,WR,IRWre;
	wire [5:0] opCode;
	wire [4:0] rd,sa,WriteReg;
    wire [2:0] ALUOp,state;
    wire [1:0] PCSrc;
	wire PCWre,RegWre,ExtSel,ALUSrcA,ALUSrcB,DBDataSrc;
	assign IAddr=curPC;
	assign Daddr=alu_out;
	
    ControlUnit CU(opCode,CLK,Reset,zero,sign,PCWre, ALUSrcA, ALUSrcB, DBDataSrc, RegWre, WrRegDSrc, InsMemRW, RD, WR, IRWre, ExtSel,
	PCSrc, RegDst,ALUOp,state);  

    PC pc(CLK,Reset,PCWre,nextPC,curPC);
	
	PC_add4 pc4(curPC, PC4);
	
	PC_select select_pc(curPC,PC4,PCSrc, ExtImme,ReadData1,addr,nextPC);

	InsMEM ROM(InsMemRW,IAddr,IDataOut);

	InsRegister IR(CLK,IRWre,IDataOut,Ins,opCode,rs,rt,rd,sa,immediate,addr);
	
	DataMEM RAM(CLK,RD,WR,Daddr,ReadData2_out,DataOut);

	mux3to1 WriReg(RegDst,rd, rt,WriteReg);
	
	RegFile REG(CLK,RegWre,rs,rt,WriteReg,WriteData,ReadData1,ReadData2);

	DataRegister ADR(CLK,ReadData1,ReadData1_out);
	
	DataRegister BDR(CLK,ReadData2,ReadData2_out);
	
	ALU alu(A,B,ALUOp,zero,sign,result);
	
	DataRegister ALUoutDR(CLK,result,alu_out);

	DataRegister DBDR(CLK,DB,DB_out);
	
	SignZeroExtend ext(immediate,ExtSel,ExtImme);

	muxA aluA(ALUSrcA,ReadData1_out,sa,A);
	
	mux2to1_32bit aluB(ALUSrcB,ExtImme,ReadData2_out,B);
  
	mux2to1_32bit db(DBDataSrc,DataOut,result,DB);
	
	mux2to1_32bit Wdata(WrRegDSrc,DB_out,PC4,WriteData);
endmodule  