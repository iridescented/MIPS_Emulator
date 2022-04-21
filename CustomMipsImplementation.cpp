#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>
using namespace std;

#define memorysize 1000
bool mode = 1;  //* 0-binary mode, 1-assembly mode
bool debug = 1; //* Enables Debug push
string imemfile = "imem.mip";
string dmemfile = "dmem.txt";
unordered_map<string, bitset<32>> pointer;

class IF
{ //! Fetch Class
public:
    bool op = 1; //* Operational state
    IF() {}
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
    bitset<32> PC = 4194304; //* Program Counter
};
struct IDControl
{ //? Control Structure to read opcode
    bool RegDst, Jump, Branch, MemRead, MemToReg, ALUOp, MemWrite, ALUSrc, RegWrite;
};
class BaseID
{ //! Base Class
public:
    bool op = 0; //* Operational state
    BaseID() { Ctrl.RegDst = Ctrl.Jump = Ctrl.Branch = Ctrl.MemRead = Ctrl.MemToReg = Ctrl.ALUOp = Ctrl.MemWrite = Ctrl.ALUSrc = Ctrl.RegWrite = 0; }

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
    bitset<6> opcode = 0, ALUControl = 0;
    bitset<5> Rs = 0, Rt = 0, Write_Reg = 0;
    bitset<16> Imm = 0;
    bitset<32> Read_Data1 = 0, Read_Data2 = 0;
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
        readPointers();
        readInstructions();
        InsMemConv();
    }
    void readPointers()
    {
        ifstream imem(imemfile);
        string str;
        int counter = 0;
        if (imem.is_open())
        {
            cout << "\n\x1B[94mPointer Memory Loaded\033[0m" << endl;
            cout << "~PointerMemory~" << endl;
            while (getline(imem, str))
            {
                if (str.find(":") != string::npos)
                {
                    str = str.substr(0, str.find(":", 0));
                    cout << 4194304 + counter * 4 << " ~ " << str << endl;
                    pointer[str] = 4194304 + counter * 4; //* 4194304 = (hex)0x00400000
                }
                else
                    counter++;
            }
            cout << "~~~~~~~~~~~~~~~" << endl;
        }
        else
        {
            cout << "\n\x1B[91mPointer Memory Load failed\033[0m" << endl;
        }
    }
    void readInstructions()
    {
        ifstream imem(imemfile);
        string str;
        int counter = 0;
        if (imem.is_open())
        {
            cout << "\n\x1B[94mInstruction Memory Loaded\033[0m" << endl;
            cout << "~InstructionMemory~~~~~~~" << endl;
            while (getline(imem, str))
            {
                str = str.substr(0, str.find("#", 0));
                if (str.size() && str.find(":") == string::npos)
                {
                    str.erase(remove(str.begin(), str.end(), ','), str.end());
                    mem.push_back(str);
                    cout << setfill('0') << setw(2) << counter << ": " << str << endl;
                    counter++;
                }
            }
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        }
        else
            cout << "\n\x1B[91mInstruction Memory Load failed\033[0m" << endl;
    }
    void InsMemConv()
    {
        for (int counter = 0; counter < mem.size(); counter++)
        {
            stringstream buffer;
            string word;
            bitset<6> funct = 0;
            bitset<16> immediate;
            vector<bitset<5>> t_Rstd;
            stringstream iss(mem[counter]);
            iss >> word;
            buffer << opcodelib[functions[word]];
            int cases = functions[word];
            switch (cases)
            {
            case add:
                funct = 32;
            case sub:
                for (int i = 0; i < 3; i++)
                {
                    iss >> word;
                    word.erase(remove(word.begin(), word.end(), ','), word.end());
                    t_Rstd.push_back(registerlib[word]);
                }
                buffer << t_Rstd[1] << t_Rstd[2] << t_Rstd[0] << bitset<5>(0);
                if (!funct.to_ulong())
                    funct = 34;
                buffer << funct;
                //(t_opcode.to_ulong() << 26 | t_Rstd[0].to_ulong() << 21 | t_Rstd[1].to_ulong() << 16 | t_Rstd[2].to_ulong() << 11 | t_shamt.to_ulong() << 6 | t_funct.to_ulong());
                break;
            case addi:
            case beq:
                for (int i = 0; i < 2; i++)
                {
                    iss >> word;
                    word.erase(remove(word.begin(), word.end(), ','), word.end());
                    t_Rstd.push_back(registerlib[word]);
                }
                buffer << t_Rstd[1] << t_Rstd[0];
                iss >> word;
                if (cases == addi)
                    immediate = stoi(word);
                else
                    immediate = pointer[word].to_ulong() - (4194304 + 4 * counter);
                buffer << immediate;
                break;
            case j:
                iss >> word;
                buffer << pointer[word];
                break;
            case jr:
                iss >> word;
                buffer << bitset<5>(registerlib[word]) << bitset<21>(8);
                break;
            default:

                break;
            }
            membin.push_back(bitset<32>(buffer.str()));
        }
        cout << "\n\x1B[94mInstruction Memory Converted\033[0m" << endl;
        cout << "~~~~InstructionMemoryBinary~~~~~~~~~" << endl;
        for (int counter = 0; counter < membin.size(); counter++)
        {
            cout << setfill('0') << setw(2) << counter << ": " << membin[counter] << endl;
        }

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    }
    vector<bitset<32>> outMem() { return membin; }

private:
    vector<bitset<32>> membin;
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
    unordered_map<string, int> registerlib = {{"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}};

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
    bool op = 0;
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

// MIPS Fields
/*

 < R-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |   rd(5bits)  | shamt(5bits) |   fucnt(6bits)  |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


 < I-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |         constant or address (16bits)          |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


 < J-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |                            address (30bits)                                 |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/