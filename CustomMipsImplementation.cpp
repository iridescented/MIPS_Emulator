#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
using namespace std;

#define memorysize 1000
bool debug = 1; // Enables Debug push

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

private:
    bitset<32> PC; //* Program Counter
};
struct IDControl
{ //? Control Structure to read opcode
    bool RegDst, Jump, Branch, MemRead, MemToReg, ALUOp, MemWrite, ALUSrc, RegWrite;
};
class ID
{ //! Decode Class
public:
    bool op; //* Operational state
    ID()
    {
        Ctrl.RegDst = Ctrl.Jump = Ctrl.Branch = Ctrl.MemRead = Ctrl.MemToReg = Ctrl.ALUOp = Ctrl.MemWrite = Ctrl.ALUSrc = Ctrl.RegWrite = 0;
        opcode = ALUControl = bitset<6>(0);
        Rs = Rt = Write_Reg = bitset<5>(0);
        Imm = bitset<16>(0);
        Read_Data1 = Read_Data2 = bitset<32>(0);
        op = 0;
    }

    void ReadInsMem(bitset<32> readAddress, bitset<32> mem)
    {
        bitset<32> Instruction = bitset<32>(mem[readAddress.to_ulong()].to_string() + mem[readAddress.to_ulong() + 1].to_string() + IMem[Read_Address.to_ulong() + 2].to_string() + IMem[Read_Address.to_ulong() + 3].to_string());
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
class EX : public ID
{ //! Execute Class=
public:
private:
};
class MEM : public ID
{ //! Memory Access Class
public:
private:
};
class WB : public ID
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
    InstructionMem()
    {
        mem.resize(memorysize);
        ifstream imem;
        string line;
        int i = 0;
        imem.open("imem.txt>");
        if (imem.is_open())
        {
            while (getline(imem, line))
            {
                mem[i] = bitset<8>(line);
                i++;
            }
        }
    }

private:
    vector<bitset<8>> mem;
};
class DataMem
{
public:
    DataMem()
    {
    }

private:
    vector<bitset<8>> mem;
};

unsigned long bitshift(bitset<32> value, int pos)
{
    return ((value.to_ulong()) >> pos);
}

int main()
{
    system("cls");
    ClearFiles();
    //! Create State and Initialize
    stateClass state, nextState;
    int loop = 0;
    bool op = 1;
    InstructionMem Imem;
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
            nextState.StoreInstructions();
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