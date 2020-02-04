module ControlUnit(
	input [5:0] opCode, //func;
	input CLK,Reset,zero,sign,
	output reg PCWre, ALUSrcA, ALUSrcB, DBDataSrc, RegWre, WrRegDSrc, InsMemRW, RD, WR, IRWre, ExtSel,
	output reg [1:0] PCSrc, RegDst,
	output reg [2:0] ALUOp,
	output reg [2:0] state
	);

parameter [5:0]
	add = 6'b000000, sub = 6'b000001, addi= 6'b000010,
	OR =  6'b010000, And = 6'b010001, ori = 6'b010010,
	sll = 6'b011000, 
	slt = 6'b100110, slti= 6'b100111,
	sw = 6'b110000, lw= 6'b110001,
	beq= 6'b110100, bne=6'b110101, bgtz= 6'b110110,
	j = 6'b111000, jr = 6'b111001, jal =6'b111010,
	halt = 6'b111111;
	
parameter[2:0]  sIF = 3'b000, // IF state
				sID = 3'b001, // ID state
				sEXE = 3'b010, // EXE state
				sMEM = 3'b100, // MEM state
				sWB = 3'b011; // WB state
reg [2:0] next_state; 	
	
	initial begin
		InsMemRW = 1;
	end
	
	always @(posedge CLK) begin  
        if (Reset == 1) begin  
            state <= sIF;  
        end 
		else begin  
            state <= next_state;  
        end  
    end
	
	always @(state or opCode) begin
		case(state)
			sIF: next_state=sID;
			sID: begin
				case(opCode[5:3])
					3'b111: next_state=sIF;
					default: next_state=sEXE;
				endcase
			end
			sEXE: begin
				if(opCode==beq || opCode==bne ||opCode==bgtz)
					next_state=sIF;
				else if(opCode==lw || opCode==sw)
					next_state=sMEM;
				else next_state=sWB;
			end
			sMEM: begin
				if(opCode==sw) next_state=sIF;
				else next_state=sWB;
			end
			sWB: next_state=sIF;
		endcase
	end
	
	always@(state or opCode or zero or sign) begin
		//PCWre
		if(state==sWB || (state==sMEM && opCode==sw))
			PCWre=1;
		else if(state==sEXE && (opCode==beq || opCode==bne || opCode==bgtz))
		    PCWre=1;
		else if (state==sID && opCode[5:2]==4'b1110)
		    PCWre=1;
		else PCWre=0;
		
		//IRWre
		if(state==sIF) IRWre=1;
		else IRWre=0;
		
		//PCSrc
		case(opCode)
			j:PCSrc=2'b11; jal:PCSrc=2'b11;
			jr:PCSrc=2'b10;
			beq: begin
				if(zero==1) PCSrc=2'b01;
				else PCSrc=2'b00;
			end
			bne: begin
				if(zero==0) PCSrc=2'b01;
				else PCSrc=2'b00;
			end
			bgtz: begin
				if(sign==0&&zero==0)
					PCSrc=2'b01;
				else PCSrc=2'b00;
			end
			default: PCSrc=2'b00;
		endcase
		
		//ExtSel
		if(opCode==ori) ExtSel=0;
		else ExtSel=1;
		
		//RegWre
		if(state==sWB || opCode==jal)
			RegWre=1;
		else RegWre=0;
		
		//RegDst
		if(opCode==jal) RegDst=2'b00;
		else if(opCode==addi || opCode==ori || opCode==slti ||opCode==lw)
			RegDst=2'b01;
		else RegDst=2'b10;
		
		//WrRegDSrc
		if(opCode==jal) WrRegDSrc=0;
		else WrRegDSrc=1;
		
		//ALUSrcA
		if(opCode==sll) ALUSrcA=1;
		else ALUSrcA=0;
		
		//ALUSrcB
		if(opCode==addi || opCode==ori || opCode==slti || opCode==lw ||opCode==sw)
			ALUSrcB=1;
		else ALUSrcB=0;
		
		//DBDataSrc
		if((state==sMEM ||state==sWB) && opCode==lw)
			DBDataSrc=1;
		else DBDataSrc=0;
		 
		//RD //WR
		if(state==sMEM) begin
			if(opCode==lw) RD=0;
			else RD=1;
			if(opCode==sw) WR=0;
			else WR=1;
		end
		
		//防止在取指令阶段写入
		if(state==sIF) begin
			RegWre=0;
			WR=1;
		end
		 
		//ALUOp
		case(opCode)
			sub: ALUOp=3'b001; OR: ALUOp=3'b101; And :ALUOp=3'b110; ori :ALUOp=3'b101;
			sll: ALUOp=3'b100; slt: ALUOp=3'b011; slti: ALUOp=3'b011;
			beq: ALUOp=3'b001; bne: ALUOp=3'b001; bgtz: ALUOp=3'b001;
			default: ALUOp=3'b000;
		endcase
	end
endmodule