
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;
int regs[32] = { 0 };
unsigned int pc = 0x0;
char memory[8 * 1024];    // only 8KB of memory located at address 0
void emitError(string s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}
bool instDecExec(unsigned int instWord)
{
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;
    
    regs[0]=0;
    unsigned int instPC = pc - 4;
    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    funct7 = (instWord >> 25) & 0x0000007F;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    // â€” inst[31] â€” inst[30:25] inst[24:21] inst[20]
    I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
    S_imm = ((instWord >> 7) | ((instWord >> 25) & 0xFE0));
    S_imm = S_imm | ((instWord >> 31) ? 0xFFFFF800 : 0x0);
    B_imm = (instWord >> 7) & 0x1E;
    B_imm = ((instWord >> 20) & 0x7E0) | B_imm;
    B_imm = ((instWord << 4) & 0x800) | B_imm;
    B_imm = B_imm | ((instWord >> 31) ? 0xFFFFF800 : 0x0);
    U_imm = (instWord & 0xFFFFF000) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
    J_imm = ((instWord >> 21) & 0x3FF);
    J_imm = ((instWord >> 10) & 0x400) | J_imm;
    J_imm = ((instWord >> 1) & 0x7F800) | J_imm;
    J_imm = ((instWord >> 13) & 0x80000) | J_imm;
    J_imm = J_imm | ((instWord >> 31) ? 0xFFFFF800 : 0x0);

 printPrefix(instPC, instWord);
    
    if (opcode == 0x33) {// R Instructions
        switch (funct3) {
            case 0: if (funct7 == 32) {
                cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] - regs[rs2];
            }
            else {
                cout << "\tADD\t" << "\tx" << dec<<rd << ", x"<<rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] + regs[rs2];
            }
                break;
            case 1: cout << "\tSLL\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] << regs[rs2];
                break;
            case 2: // Set less than
                cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                if (regs[rs1] < regs[rs2])
                    regs[rd] = 1;
                else
                    regs[rd] = 0;
                break;
            case 3: //Set less than unsigned
                cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                if ((unsigned)regs[rs1] < (unsigned)regs[rs2])
                    regs[rd] = 1;
                else
                    regs[rd] = 0;
                break;
            case 4: // XOR
                cout << "\tXOR\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] ^ regs[rs2];
                break;
            case 5: //Shift right
                if (funct7 == 32) { //shift right logical
                    cout << "\tSRL\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    regs[rd] = regs[rs1] >> regs[rs2];
                }
                else { //shift right arthmetic
                    cout << "\tSRA\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    regs[rd] = (signed)regs[rs1] >> regs[rs2];
                }
            case 6: // OR
                cout << "\tOR\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] | regs[rs2];
                break;
            case 7: //AND
                cout << "\tAND\t" << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                regs[rd] = regs[rs1] & regs[rs2];
                break;
            default:
                cout << "\tUnkown R Instruction \n";
        }
    }
    else if (opcode == 0x13) {    // I instructions
        switch (funct3) {
            case 0: cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                    regs[rd] = regs[rs1] + (int)I_imm;
                break;
            case 1:  cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                     I_imm = I_imm >> 5;
                     regs[rd] = regs[rd] << (int)I_imm;
                break;
            case 2:    cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                regs[rd] = (int)regs[rs1] < (int)I_imm ? 1 : 0;
            case 3:    cout << "\tSLTIU\tx" << rd << ", x" << (unsigned int)rs1 << ", " << hex << "0x" << (unsigned int)I_imm << "\n";
                regs[rd] = (unsigned int)regs[rs1] < (unsigned int)I_imm ? 1 : 0;
                break;
            case 4:    cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                regs[rd] = regs[rs1] ^ (int)I_imm;
                break;
            case 5:
                I_imm = I_imm >> 5;
                if (I_imm == 0)
                {
                    cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                    regs[rd] = (unsigned)regs[rd] >> (int)I_imm;
                }
                else
                {
                    cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                    regs[rd] = (signed)regs[rd] >> (int)I_imm;
                }
                break;
            case 6:    cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                regs[rd] = regs[rs1] | (int)I_imm;
                break;
            case 7:    cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << dec << (int)I_imm << "\n";
                regs[rd] = regs[rs1] & (int)I_imm;
                break;
            default:
                cout << "\tUnkown I Instruction \n";
        }
    }
    else if (opcode == 0x3) {    //Load instructions in I instructions
        switch (funct3) {
            case 0:    cout << "\tLB\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                regs[rd] = (memory[(regs[rs1] + I_imm)] & 0xFF);
                break;
                
            case 1:cout << "\tLH\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                regs[rd] = ((memory[(regs[rs1] + I_imm)+1] <<8) |(memory[regs[rs1]+I_imm]));
                break;
                
            case 2:  cout << "\tLW\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                regs[rd] = ((memory[(regs[rs1] + I_imm)+3] <<24) |((memory[(regs[rs1] + I_imm)+2] <<16)))|(((memory[(regs[rs1] + I_imm)+1]<<8)| (memory[(regs[rs1]+I_imm)])));
                break;
            case 4: cout << "\tLBU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                regs[rd] = (unsigned)(memory[(regs[rs1] + I_imm)] & 0xFF);
                
                break;
            case 5:  cout << "\tLHU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                regs[rd] = (unsigned)(memory[(regs[rs1] + I_imm)] & 0xFF);
                break;
            default:
                cout << "\tUnkown I Instruction \n";
        }
    }
    else if (opcode == 0x23) { // S instruction
        
        int shift; //
        switch (funct3) {
            case 0: // store byte
                cout << "\tSB\tx" << "\tx" << ", x" << rs2 << ", " << S_imm << "), (" << rs1 << ")\n";
                memory[S_imm + regs[rs1]] = (regs[rs2] & 0xFF);
                break;
            case 1://store half word
                cout << "\tSH\tx" << "\tx" << ", x" << rs2 << ", " << S_imm << "), (" << rs1 << ")\n";
                memory[S_imm + regs[rs1]] = (regs[rs2] & 0xFF);
                memory[S_imm + regs[rs1] + 1] = (regs[rs2] >> 8) & 0xFF;
                break;
            case 2://store word
                cout << "\tSW\tx" << "\tx" << ", x" << rs2 << ", " << S_imm << "), (" << rs1 << ")\n";
                shift = 0;
                for (int i = 0; i < 4; i++)
                {
                    memory[S_imm + regs[rs1] + i] = (regs[rs2] >> shift & 0xFF);
                    shift += 8;
                }
                break;
            default:
                cout << "\tUnkown S Instruction \n";
        }
    }
    else if (opcode == 0x63)// B instruction
    {
        switch (funct3)
        {
            case 0: // Branch equal
                cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << B_imm << "\n";
                if (regs[rs1] == regs[rs2])
                    pc += B_imm - 4;
                break;
            case 1: //Branch not equal
                cout << "\tBNE\t" << "x" << rs1 << ", x" << rs2 << ", " << hex << B_imm << "\n";
                if (regs[rs1] != regs[rs2])
                    pc += B_imm - 4;
                break;
            case 4: //Branch less than
                cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << B_imm << "\n";
                if (regs[rs1] < regs[rs2])
                    pc += B_imm - 4;
                break;
            case 5: //Branch greater than or equal
                cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << B_imm << "\n";
                if (regs[rs1] >= regs[rs2])
                    pc += B_imm - 4;
                break;
            case 6: //Branch less than unsigned
                cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << B_imm << "\n";
                if ((unsigned)regs[rs1] < (unsigned)regs[rs2])
                    pc += B_imm;
                break;
            case 7: //Branch greater than or equal
                cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << B_imm << "\n";
                if ((unsigned)regs[rs1] >= (unsigned)regs[rs2])
                    pc += B_imm - 4;
                break;
            default:
                cout << "\tUnkown B Instruction \n";
        }
    }
    else if (opcode == 0x6F)// JAL Instruction
    {
        if(rd==0)
            regs[rd]=0;
        else
            regs[rd]=pc+4;
            
        pc +=(J_imm*2)-4;
        cout << "\tJAL\tx" << rd << ", " << J_imm << "\n";
    }
    
    else if(opcode==0x67)// JALR instruction
    {
        regs[rd]=pc+4;
        pc=(regs[rs1]+I_imm)-4;
        cout << "\tJALR\tx" <<rs1 <<",x"<< rd << ", " << I_imm << "\n";
    }
    
    else if (opcode == 0x73) {// ecall
        int i=0,k;
        char c;
        string s;
        switch (regs[17])
        {
            case 1: 
                cout <<"\t The output is "<< dec << regs[10] << endl;
                break;
            case 4:
                i=regs[10];
                cout<<"\t";
                while(memory[i]!='0')
                {
                    cout<<memory[i++];
                }
                cout<<endl;
                break;
            case 5:
                cout<<"\t Input the number"<<endl;
                cin >> regs[10];
    
                break;
                
            case 8:
                cin>>regs[10];// string
                cin>>regs[11];// string length
                
            case 10:
                cout << " \tthe program has ended\n";
                return false;
                break;
                
//            default:
//                cout << " \tUnknown Ecall \n";
        }
    }
    else
        if(opcode==0x37) // LUI
        {
            regs[rd]=U_imm;
            
            cout << "\tLUI\tx" <<rd << ", " << U_imm << "\n";
        }
        else
            if(opcode==0x2F)// AUIPC
            {
                regs[rd]=pc+U_imm;
                cout << "\tAUIPCI\tx" <<rd << ", " << U_imm << "\n";
            }
            else
                
                cout << "\tUnkown  Instruction \n";
    return true;
}
// compressed
bool instDecCompExec(unsigned int instWord) {
    regs[0]=0;
    unsigned int rdC, rs1C,rs2C, funct2C, funct3C,TEMP, opcodeC,U_immC,I_immC,Z_immC;
    I_immC=(((instWord>>2)&0x001F)|((instWord>>7)&0x0020));
     U_immC=((((instWord>>6)&0x0001)|((instWord>>9)&0x0007))|((instWord>>1)&0x0010));
    Z_immC=(instWord>>2&0x001F);
    unsigned int instPC = pc - 4;
    opcodeC = instWord & 0x3;
    funct3C = (instWord>>13) & 0x3;
    printPrefix(instPC, instWord);
    if (opcodeC == 0x0) {
        switch (funct3C)
        {
            case 2:rdC=(instWord>>2)&0x3;
                rs1C=(instWord>>7)&0x3;
                cout << "\tC.LW\tx" << rdC << ", x" << rs1C << ", " << dec << (int)U_immC << "\n";
                regs[rdC] = ((memory[(regs[rs1C] + U_immC)+1] <<8) |(memory[regs[rs1C]+U_immC]));
                break;
                
            case 6:
                rs2C=(instWord>>2)&0x3;
                rs1C=(instWord>>7)&0x3;
                cout << "\tC.SW\tx" << rs2C << ", x" << rs1C << ", " << dec << (int)U_immC << "\n";
                memory[U_immC + regs[rs1C]] = (regs[rs2C] & 0xFF);
                memory[U_immC + regs[rs1C] + 1] = (regs[rs2C] >> 8) & 0xFF;
                break;
        }
    }
    else if(opcodeC==0x1){
        switch(funct3C)
        {
            case 0:
                rdC=(instWord>>7)&0x001F;
                rs2C=(instWord>>7)&0x001F;
                if(rdC==0)
                {
                    cout << "\tC.NOP\tx" << rdC << ", x" << rs1C << ", " << dec << (int)I_immC << "\n";
                    regs[rdC]=regs[0]+regs[0];
                }
                else if(rdC!=0)
                {
                cout << "\tC.ADDI\tx" << rdC << ", x" << rdC << ", " << dec << (int)I_immC << "\n";
                regs[rdC]=regs[rdC]+I_immC;
                }
                else if(rs1C==0)
                {
                cout << "\tC.NOP\tx" << rs1C << ", x" << rs1C << ", " << dec << (int)I_immC << "\n";
                regs[rs1C]=regs[0]+regs[0];
                }
                else
                {
                cout << "\tC.ADDI\tx" << rs1C << ", x" << rs1C << ", " << dec << (int)I_immC << "\n";
                regs[rs1C]=regs[rs1C]+I_immC;
                }
                    break;
                    
            case 1:
                cout << "\tC.JAL\tx" <<endl;
                break;

            case 2:
                rdC=(instWord>>7)&0x001F;
                rs2C=(instWord>>7)&0x001F;
                if(rdC!=0)
                {
                 cout << "\tC.LI\tx" << rdC << ", x" << rdC << ", " << dec << (int)I_immC << "\n";
                 regs[rdC]=I_immC+0;
                }
                    
                else
                {
                cout << "\tC.LI\tx" << rs1C << ", x" << rs1C << ", " << dec << (int)I_immC << "\n";
                regs[rs1C]=I_immC+0;
                }
                break;
                    
            case 3:
                cout << "\tC.LUI\tx" <<endl;
                 break;
               
            case 4:
                TEMP=(instWord>>12)&0x0007;
                rdC=(instWord>>7)&0x001F;
                rs2C=(instWord>>7)&0x001F;
                    if(TEMP==0)
                    {
                        cout << "\tC.SRLI\tx" << rdC << ", x" << rs1C << ", " << dec << (int)Z_immC << "\n";
                        regs[rdC]=regs[rs1C]>>Z_immC;
                    }
                       
                   else if(TEMP==1)
                   {
                       cout << "\tC.SRAI\tx" << rdC << ", x" << rs1C << ", " << dec << (int)Z_immC << "\n";
                       regs[rdC]=(signed)regs[rs1C]>>Z_immC;
                   }
                    else if(TEMP==2)
                    {
                        cout << "\tC.ANDI\tx" << rdC << ", x" << rs1C << ", " << dec << (int)Z_immC << "\n";
                        regs[rdC]=Z_immC&regs[rdC];
                        
                    }
                        break;
            }
        }
    else
        cout<<"Unsupported compressed function"<<endl;
    return true;
    }



int main(int argc, char *argv[])
{
    unsigned int instWord = 0;
    ifstream inFile;
    ofstream outFile;
        if(argc<1) emitError("use: rv32i_sim <machine_code_file_name>\n");
        inFile.open(argv[1], ios::in | ios::binary | ios::ate);
//    inFile.open("/Users/ahmedibrahim/Desktop/samples/div.bin", ios::in | ios::binary | ios::ate);
    if (inFile.is_open())
    {
        bool flag;
        int fsize = inFile.tellg();
        inFile.seekg(0, inFile.beg);
        if (!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");
        while (true) {
            
            instWord = (unsigned char)memory[pc] |
            (((unsigned char)memory[pc + 1]) << 8) |
            (((unsigned char)memory[pc + 2]) << 16) |
            (((unsigned char)memory[pc + 3]) << 24);
            pc += 4;
            if ((instWord & 3) == 3)
            {
                flag = instDecExec(instWord);
                if (!flag)
                    break;
            }
            else
            {
                flag = instDecCompExec(instWord & 0xFFFF0000);
                if (!flag)
                    break;
                flag = instDecCompExec(instWord & 0x0000FFFF);
                if (!flag)
                    break;
            }
            
        }
        // dump the registers
        for (int i = 0; i < 32; i++)
            cout << "x" << dec << i << ": \t" << "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";
    }
    else emitError("Cannot access input file\n");
    return 0;
}