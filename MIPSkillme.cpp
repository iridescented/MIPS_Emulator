/*
Name: Leeland Raj Kumar
Southampton ID: 32492189
Project: MIPS Processor

*/

#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include<Windows.h>
using namespace std;
#define MemSize 1000

struct F_Struct { //!Fetch Structure
    bitset<32> PC; //*Program Counter
    bool active; //*Determines whether Structure is Active
};

struct D_Struct { //!Decode Structure
    bitset<32> Ins_Mem; //*Instruction Memory
    bool active; //*Determines whether Structure is Active
};

struct E_Struct { //!Execute Structure
    bitset<5>   Rs; //*Read Register 1
    bitset<5>   Rt; //*Read Register 2
    bitset<5>   Write_Reg;  //*Write Register
    bitset<32>  Read_Data1; //*Read Data 1
    bitset<32>  Read_Data2; //*Read Data 2
    bitset<16>  Imm;
    bool wrt_enable; //*Write Enabler (From Write_Data)
    bool active; //*Determines whether Structure is Active
    /* 
    ? bool        is_I_type;
    ? bool        rd_mem;
    ? bool        wrt_mem; 
    ? bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    */
};

struct MS_Struct { //!MemoryAccess Structure
    bitset<5>   Rs; //*Read Register 1
    bitset<5>   Rt; //*Read Register 2
    bitset<5>   Write_Reg;  //*Write Register
    bitset<32>  ALU_Result;
    bool wrt_enable; //*Write Enabler (From Write_Data)
    bool active; //*Determines whether Structure is Active
    /*
    ? bitset<32>  Store_data;
    ? bool        rd_mem;
    ? bool        wrt_mem; 
    ? bool        wrt_enable;
    */
};

struct WB_Struct { //!WriteBack Structure
    bitset<32>  Wrt_Data;
    bitset<5>   Rs; //*Read Register 1
    bitset<5>   Rt; //*Read Register 2
    bitset<5>   Write_Reg;  //*Write Register
    bool wrt_enable; //*Write Enabler (From Write_Data)
    bool active; //*Determines whether Structure is Active
};

struct stateStruct { //!State Structure to access all values
    string name;
    int loopstate;
    F_Struct F;
    D_Struct D;
    E_Struct E;
    WB_Struct W;
    MS_Struct M;
};

stateStruct initialization(string name) {
    stateStruct s;
    s.F.PC = bitset<32> (0);
    s.E.Rs = s.E.Rt = s.E.Write_Reg = s.M.Rs = s.M.Rt = s.M.Write_Reg = s.W.Rs = s.W.Rt = s.W.Write_Reg = bitset<5> (0);
    s.D.Ins_Mem = s.E.Read_Data1 = s.E.Read_Data2 = s.M.ALU_Result = s.W.Wrt_Data = bitset<32> (0);
    s.E.Imm = bitset<16> (0);
    s.E.wrt_enable = s.M.wrt_enable = s.W.wrt_enable = s.D.active = s.E.active = s.M.active = s.W.active = s.loopstate = 0;
    s.F.active =1;
    s.name = name;
    ofstream file("OutputTest.txt", std::ios_base::app);
    if(file.is_open())
        file << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;
    else
        cout << "\nOutputTest.txt failed to open!";file.close();
    return s;
}

void ClearFiles() {
    remove("OutputTest.txt");
}

void printCurrentState(stateStruct s,int loop) {
    ofstream file("OutputTest.txt", std::ios_base::app); //https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
    if(file.is_open()) {
        //file << "Outputting Current State of Structure ~(" << s.name << ")~\n";
        file <<"~~~~~~~~~~\n|Loop: "<<loop<<" |\n~~~~~~~~~~\n\n";

        file << "~F.PC dec      : "<<s.F.PC.to_ulong()<<endl;
        file << "~F.PC bin      : "<<s.F.PC<<endl<<endl;

        file << "~D.Ins_Mem   : "<<s.D.Ins_Mem<<endl<<endl;

        file << "~E.Rs          : "<<s.E.Rs<<endl;
        file << "~E.Rt          : "<<s.E.Rt<<endl;
        file << "~E.Write_Reg   : "<<s.E.Write_Reg<<endl;
        file << "~E.Read_Data1  : "<<s.E.Read_Data1<<endl;
        file << "~E.Read_Data2  : "<<s.E.Read_Data2<<endl;
        file << "~E.wrt_enable  : "<<s.E.wrt_enable<<endl<<endl;

        file << "~M.Rs          : "<<s.E.Rs<<endl;
        file << "~M.Rt          : "<<s.E.Rt<<endl;
        file << "~M.Write_Reg   : "<<s.E.Write_Reg<<endl;
        file << "~M.ALU_Result  : "<<s.M.ALU_Result<<endl;
        file << "~M.wrt_enable  : "<<s.E.wrt_enable<<endl<<endl;

        file << "~W.Rs          : "<<s.E.Rs<<endl;
        file << "~W.Rt          : "<<s.E.Rt<<endl;
        file << "~W.Write_Reg   : "<<s.E.Write_Reg<<endl;
        file << "~W.Wrt_Data    : "<<s.W.Wrt_Data<<endl;
        file << "~W.wrt_enable  : "<<s.E.wrt_enable<<endl<<endl;

        file << "~ACTIVESTAGES~\n~|IF |ID | E |MS |WB |~"<<endl;
        file << "~| "<<s.F.active<<" | "<<s.D.active<<" | "<<s.E.active<<" | "<<s.M.active<<" | "<<s.W.active<<" |~"<<endl;
        file << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;
    }
    else
        cout << "\nOutputTest.txt failed to open!";
    file.close();
}

class InstructionMemory { //? COPIED DONT FORGET CHANGE
    public:
        bitset<32> Instruction;
        InstructionMemory()
        {       
            IMem.resize(MemSize); 
            string line;
            int i=0;
            ifstream imem("imem.txt");
            if (imem.is_open())
            {
                while (getline(imem,line))
                {      
                    IMem[i] = bitset<8>(line);
                    i++;
                }                    
            }
            else cout<<"Unable to open file";
            imem.close();                     
        }
        bitset<32> readIM(bitset<32> Read_Address) {
            string insmem = IMem[Read_Address.to_ulong()].to_string() + IMem[Read_Address.to_ulong()+1].to_string() + IMem[Read_Address.to_ulong()+2].to_string() + IMem[Read_Address.to_ulong()+3].to_string();
            bitset<32> Instruction = bitset<32>(insmem);		//read instruction memory
            return Instruction;   
        }
    private:
        vector<bitset<8> > IMem;
};

unsigned long ShiftB(bitset<32> value, int pos)
{
    return ((value.to_ulong())>>pos);
}

int main() {
    system("cls");
    //!Creating State
    stateStruct State, New_State;
    //!Initialization
    ClearFiles();
    bool active = 1;
    bitset<32> Ins = bitset<32> (0);
    InstructionMemory mainInsMem;
    New_State = State = initialization("State");
    printCurrentState(State,State.loopstate);

    //!MainCycle
    while(active) {
        //WriteBack
        if(State.W.active) {

        }
        //MemoryAccess
        New_State.W.active = State.M.active;
        if(State.M.active) {

        }
        //Execute
        New_State.M.active = State.E.active;
        if(State.E.active) {

        }
        //Decode
        New_State.E.active = State.D.active;
        if(State.D.active) {
            Ins = State.D.Ins_Mem;
            New_State.E.Rs = bitset<5> (ShiftB(Ins, 21));
            New_State.E.Rt = bitset<5> (ShiftB(Ins, 16));
            New_State.E.Imm = bitset<16> (ShiftB(Ins, 0)); //? extend?
            //? Reg Addr
        }
        //Fetch
        New_State.D.active = State.F.active;
        if(State.F.active) {
            New_State.D.Ins_Mem = mainInsMem.readIM(State.F.PC);
            New_State.F.PC = State.F.PC.to_ulong()+4;
            //! INCLUDE END STATE
            if(New_State.D.Ins_Mem == bitset<32>(4294967296)) { //2^32
                New_State.F.PC=State.F.PC;
                New_State.F.active = New_State.D.active = 0;
            }
        }

        New_State.loopstate++;
        State = New_State;
        printCurrentState(State,State.loopstate);
        
        if(!State.F.active&&!State.D.active&&!State.E.active&&!State.M.active&&!State.W.active) {
            active = 0;
        }
    }
    return 0;   
}