`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/01/2019 02:29:41 PM
// Design Name: 
// Module Name: datapath
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module a32mux (input[31:0] a,b,input sel,output [31:0]out); 
genvar i; generate     for(i=0;i<32;i=i+1) 
       a2x1mux m (a[i],b[i],sel,out[i]); 
endgenerate 
 endmodule

 exp4 module Imm_generator( input[31:0] address, output reg[31:0] imm);
 wire[6:0] opcode = address[6:0]; 
wire[31:0] I_imm=((address >>20)&12'h7FF)| ((address>>31==0)?32'h0:32'hFFFFF800); wire[31:0] S_imm={{20{address[31]}},address[31:25],address[11:7]}; wire[31:0]
 B_imm = {{20{address[31]}},address[31],address[7],address[30:25],address[11:8],1'b0}; 
 always@(*)  begin if((opcode==7'b0010011)||(opcode==7'b0000011))// I_immediate     imm=I_imm; 
    else     
if(opcode==7'b0100011)// S_immediate 
    imm=S_imm; 
  
  else     
if(opcode==7'b1100011)// B_immediate    
     imm=B_imm;  
end 
endmodule



module data_path(input clk,
input clk_ssd,
input rst, 
input [1:0] ledSel, 
input[3:0] ssdSel,
output reg [0:3] Anode , 
output [15:0] leds, 
output reg [0:6] LED_out );
module processor(input clk, rst);
    wire [31:0] PCin,PCout ,RDout,writedata ,inst,rData1, rData2, aluMUX_out,dmemMUX_out,alu_out, imm, data_out;
    wire [32:0] branch_sum, PC_counter;
    wire [2:0] ALUop;
    wire regWrite, memRead, MemToReg, memWrite,Branch,ALUsrc,output_branch, terminate, carryFlag, zeroFlag, overflowFlag, signFlag;
    wire [3:0] ALUsel;
    wire [31:0]addMUX1_out, MUX_out;
    
output [31:0] readdata1, readdata2 );

PC pc(clk, rst, 1,PCin, PCout);
InstMem instmem(PCout[0:5], inst);
RegFile rg(clk,rst,instr[19-15],instr[24-20],instr[11-7],writedata,regWrite,rData1,rData2)
a32mux aluMux(imm,rData2, ALUsrc,aluMUX_out);
ALU32bit alu (rData1, aluMUX_out,ALUsel,alu_out, zeroFlag );
DataMem datamemory (clk, memRead, MemWrite,input [5:0] addr, rData2, data_out);
DataMem datamem(clk, memRead, inst[`IR_funct3],memWrite, alu_out, rData2, data_out);
Imm_generator Imm(inst, imm);

Rd_control RD(inst[`IR_opcode],immRes, PC_counter[31:0], branch_sum[31:0], dmemMUX_out, RDout);
Control_unit CU(inst[`IR_opcode], terminate, regWrite, memRead, MemToReg, memWrite,Branch,ALUsrc,ALUop,jump );
ALU_controlUnit ALU_CU( ALUop, inst[`IR_funct3],inst[30], ALUsel);
Branch_control BC( inst[`IR_funct3], Branch,zeroFlag, overflowFlag, carryFlag, signFlag, output_branch);

Adder PC_add(PCout,4,0,PC_counter);
Adder branch_add(PCout,immRes,0,branch_sum);
MUX32 PC_MUX(PCout,MUX_out,terminate,PCin);
Adder PC_add(PCout,4,0, PC_counter);
Adder branch_add(PCout,immRes,0, branch_sum);
MUX32 PC_MUX (PCout,MUX_out, terminate,PCin);
MUX32 ALU_MUX (immRes,rData2, ALUsrc,aluMUX_out);
MUX32 dMem_MUX (data_out,alu_out, MemToReg,dmemMUX_out);
MUX32 adderMUX1 (branch_sum,PC_counter, output_branch,addMUX1_out);
MUX32_3 MUX(addMUX1_out,branch_sum,alu_out, jump, MUX_out);





endmodule