#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>
using namespace std;

#define memorysize 1000
bool mode = 1;  //* 0-binary mode, 1-assembly mode
bool debug = 1; //* Enables Debug push
string imemfile = "imem.mip";
string dmemfile = "dmem.txt";

class IF
{ //! Fetch Class
public:
    bool op; //* Operational state
    IF()
    {
        PC = bitset<32>(0);
        op = 1;
    }
    void countUp(int inc)
    {
        PC = PC.to_ulong() + inc;
    }
    void print()
    {
        ofstream file("OutputTest.txt", std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~Fetch Stage~~~~" << endl;
            file << "~PC dec      : " << PC.to_ulong() << endl;
            file << "~PC bin      : " << PC << endl
                 << endl;
        }
        else
            cout << "\nOutputTest.txt failed to open!";
        file.close();
    }
    bitset<32> PCOut()
    {
        return PC;
    }

private:
    bitset<32> PC; //* Program Counter
};
struct IDControl
{ //? Control Structure to read opcode
    bool RegDst, Jump, Branch, MemRead, MemToReg, ALUOp, MemWrite, ALUSrc, RegWrite;
};
class BaseID
{ //! Base Class
public:
    bool op; //* Operational state
    BaseID()
    {
        Ctrl.RegDst = Ctrl.Jump = Ctrl.Branch = Ctrl.MemRead = Ctrl.MemToReg = Ctrl.ALUOp = Ctrl.MemWrite = Ctrl.ALUSrc = Ctrl.RegWrite = 0;
        opcode = ALUControl = bitset<6>(0);
        Rs = Rt = Write_Reg = bitset<5>(0);
        Imm = bitset<16>(0);
        Read_Data1 = Read_Data2 = bitset<32>(0);
        op = 0;
    }

    unsigned long bitshift(bitset<32> value, int pos)
    {
        return ((value.to_ulong()) >> pos);
    }

    void print(string stage)
    {
        ofstream file("OutputTest.txt", std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~" << stage << " Stage~~~~" << endl;
            file << "~Rs          : " << Rs << endl;
            file << "~Rt          : " << Rt << endl;
            file << "~Write_Reg   : " << Write_Reg << endl;
            file << "~Read_Data1  : " << Read_Data1 << endl;
            file << "~Read_Data2  : " << Read_Data2 << endl;
            file << "~Control Data: " << Ctrl.RegDst << " " << Ctrl.Jump << " " << Ctrl.Branch << " " << Ctrl.MemRead << " " << Ctrl.MemToReg << " " << Ctrl.ALUOp << " " << Ctrl.MemWrite << " " << Ctrl.ALUSrc << " " << Ctrl.RegWrite << "~" << endl
                 << endl;
        }
        else
            cout << "\nOutputTest.txt failed to open!";
        file.close();
    }

protected:
    IDControl Ctrl;
    bitset<6> opcode, ALUControl;
    bitset<5> Rs, Rt, Write_Reg;
    bitset<16> Imm;
    bitset<32> Read_Data1, Read_Data2;
};
class ID : public BaseID
{ //! Execute Class=
public:
    void readInsMem(bitset<32> readAddress, vector<bitset<8>> mem)
    {
        InstructionMem = bitset<32>(mem[readAddress.to_ulong()].to_string() + mem[readAddress.to_ulong() + 1].to_string() + mem[readAddress.to_ulong() + 2].to_string() + mem[readAddress.to_ulong() + 3].to_string());
    }
    bool isEnd()
    {
        return (InstructionMem == bitset<32>(4294967295)); //* 2^32
    }
    void controlCalculations()
    {
        opcode = bitset<6>(bitshift(InstructionMem, 26));
        if (opcode.to_ulong() == 0) //* R-type with opcode 000000
        {
        }
    }

    void readInsMem2()
    {
    }

private:
    unordered_map<string, bitset<32>> bitfields;
    bitset<32> InstructionMem;
};
class EX : public BaseID
{ //! Execute Class=
public:
private:
};
class MEM : public BaseID
{ //! Memory Access Class
public:
private:
};
class WB : public BaseID
{ //! Write Back Class
public:
private:
};

class stateClass
{ //! State Class
public:
    IF F;
    ID D;
    EX E;
    MEM M;
    WB W;
    void print(int loop)
    {
        ofstream file("OutputTest.txt", std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            file << "~~~~~~~~~~\n|Loop: " << loop << " |\n~~~~~~~~~~\n";
            file << "~opSTAGES~\n~|IF |ID | E |MS |WB |~" << endl;
            file << "~| " << F.op << " | " << D.op << " | " << E.op << " | " << M.op << " | " << W.op << " |~" << endl;
        }
        else
            cout << "\nOutputTest.txt failed to open!";
        file.close();
        if (debug)
        {
            F.print();
            D.print("Decode");
            E.print("Execute");
            M.print("Nemory Access");
            W.print("Write Back");
        }
    }

private:
protected:
};
void ClearFiles()
{
    remove("OutputTest.txt");
}

class InstructionMem
{
public:
    InstructionMem() // TODO change to read words
    {
        ifstream imem(imemfile);
        int i = 0;
        string str;
        mem.resize(memorysize);
        int i = 0;
        if (imem.is_open())
        {
            while (getline(imem, str))
            {
                str = str.substr(0, str.find("#", 0));
                if (str.size())
                {
                    // str.erase(std::remove(str.begin(), str.end(), ','), str.end());
                    mem.push_back(str);
                }
            }
            for (i = 0; i < mem.size(); i++)
            {
                cout << i << ": " << mem[i] << endl;
            }
            cout << "\n\x1B[94mInstruction Memory Loaded\033[0m" << endl;
        }
        else
        {
            cout << "\n\x1B[91mInstruction Memory Load failed\033[0m" << endl;
        }
    }

    vector<bitset<32>> InsMemConv(int i)
    {
        switch ()
    }
    vector<string> outMem()
    {
        return mem;
    }

private:
    vector<string> mem;
    enum functionsenum
    {
        add,
        addi,
        sub,
        j,
        jr,
        beq
    };
    unordered_map<string, int> functions = {{"add", 0}, {"addi", 1}, {"sub", 2}, {"j", 3}, {"jr", 4}, {"beq", 5}};
    vector<bitset<6>> opcodelib = {0, 8, 0, 2, 0, 4};
};
class DataMem
{
public:
    DataMem() // TODO change to read words
    {
        mem.resize(memorysize);
        ifstream dmem;
        string str;
        int i = 0;
        dmem.open(imemfile);
        if (dmem.is_open())
        {
            cout << "\x1B[94mData Memory Loaded\033[0m" << endl;
            while (getline(dmem, str))
            {
                // mem[i] = bitset<8>(str);
                i++;
            }
        }
        else
            cout << "\x1B[91mData Memory Load failed\033[0m" << endl;
    }

private:
    vector<bitset<8>> mem;
};

int main()
{
    system("cls");
    ClearFiles();
    //! Create State and Initialize
    stateClass state, nextState;
    int loop = 0;
    bool op = 1;
    InstructionMem IMEM;
    DataMem DMEM;
    //! MainCycle

    while (op)
    {
        // WriteBack
        if (state.W.op)
        {
        }
        // MemoryAccess
        nextState.W.op = state.M.op;
        if (state.M.op)
        {
        }
        // Execute
        nextState.M.op = state.E.op;
        if (state.E.op)
        {
        }
        // Decode
        nextState.E.op = state.D.op;
        if (state.D.op)
        {
        }
        // Fetch
        nextState.D.op = state.F.op;
        if (state.F.op)
        {
            // nextState.D.readInsMem(state.F.PCOut(), IMEM.outMem());
            // nextState.D.controlCalculations();
            // if (nextState.D.isEnd())
            // {
            //     nextState.F.op = nextState.D.op = 0;
            // }
            // else
            //     nextState.F.countUp(4);
        }

        loop++;
        state = nextState;
        state.print(loop);

        if (!state.F.op && !state.D.op && !state.E.op && !state.M.op && !state.W.op)
        {
            op = 0;
        }
        op = 0;
    }
    return 0;
}