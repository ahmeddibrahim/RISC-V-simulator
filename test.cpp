#include <iostream>
#include <vector>
#include <fstream>
#include "c.cpp"
#include <string>
using namespace std;
int main()
{
    // // string s1="/Users/ahmedibrahim/Desktop/projects/RISC-V_SIM/c";
    // // Parser p(s1);
    // // p.Get_Instructions();
    //         // LW 1 2 3
    // string inst= "J 3";
    // int rs1,rs2, pos,imm;
    // string i= (inst.substr(0,2));
    // inst.erase(0,2);
    // // pos=inst.find(",");
    // cout<<inst<<endl;
    // // rs1=inst;
    // // rs1=stoi(inst.substr(0,pos));
    // // inst.erase(0,pos+2);
    // // pos=inst.find(",");
    // // rs2=stoi(inst.substr(0,pos));
    // // // cout<< inst<<endl;
    // // inst.erase(0,pos+2);
    // // imm=stoi(inst);
    // // cout<< "inst: "<<i<<endl<<"rs1: "<<rs1<<endl<<"rs2: "<<rs2<<endl<<"imm: "<<imm<<endl;
    string infile="/Users/ahmedibrahim/Desktop/projects/RISC-V_SIM/c";
    Parser p(infile);
    p.Get_Instructions();

return 0;
}

    

    

