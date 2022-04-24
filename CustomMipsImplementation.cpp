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
#define insmemstart 4194304
#define datamemstart 268500992
bool mode = 1; //* 0-binary mode, 1-assembly mode
int debug = 0; //* Enables Debug push
vector<bitset<32>> registers;
string imemfile = "imem.s";
string dmemfile = "dmem.txt";
unordered_map<string, bitset<32>> pointer;
string error = "\x1B[91mERROR\n";
/************************ FUNCTIONS & CLASSES ************************/
void ClearFiles();

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
    bitset<32> PC = insmemstart; //* Program Counter
};
struct IDControl
{ //? Control Structure to read opcode
    bool RegDst, Jump, Branch, MemRead, MemToReg, MemWrite, ALUSrc, RegWrite;
    bitset<2> ALUOp;
};
struct RegStruct
{
    IDControl Ctrl;
    bitset<6> opcode = 0, ALUControl = 0;
    bitset<5> Rs = 0, Rt = 0, Write_Reg = 0, Shamt = 0;
    bitset<16> Imm = 0;
    bitset<32> Read_Data1 = 0, Read_Data2 = 0, ALUOut = 0, Imm_se = 0;
};
class BaseID
{ //! Base Class
public:
    bool op = 0; //* Operational state
    BaseID()
    {
        R.Ctrl.RegDst = R.Ctrl.Jump = R.Ctrl.Branch = R.Ctrl.MemRead = R.Ctrl.MemToReg = R.Ctrl.MemWrite = R.Ctrl.ALUSrc = R.Ctrl.RegWrite = 0;
        R.Ctrl.ALUOp = 0;
    }
    void print(string stage)
    {
        ofstream file("OutputTest.txt", std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~" << stage << " Stage~~~~" << endl;
            file << "~Rs          : " << R.Rs << endl;
            file << "~Rt          : " << R.Rt << endl;
            file << "~Imm         : " << R.Imm << endl;
            file << "~Write_Reg   : " << R.Write_Reg << endl;
            file << "~Read_Data1  : " << R.Read_Data1 << endl;
            file << "~Read_Data2  : " << R.Read_Data2 << endl;
            file << "~Control Data: " << R.Ctrl.RegDst << " " << R.Ctrl.Jump << " " << R.Ctrl.Branch << " " << R.Ctrl.MemRead << " " << R.Ctrl.MemToReg << " " << R.Ctrl.ALUOp << " " << R.Ctrl.MemWrite << " " << R.Ctrl.ALUSrc << " " << R.Ctrl.RegWrite << "~" << endl
                 << endl;
        }
        else
            cout << "\nOutputTest.txt failed to open!";
        file.close();
    }
    RegStruct outReg()
    {
        return R;
    }
    void inReg(RegStruct Rin)
    {
        R = Rin;
    }

protected:
    RegStruct R;
};
class ID : public BaseID
{ //! Execute Class=
public:
    void readInsMem(bitset<32> readAddress, vector<bitset<32>> mem)
    {
        InstructionMem = bitset<32>(mem[readAddress.to_ulong()]);
    }
    bool isEnd()
    {
        return (InstructionMem == bitset<32>(4294967295)); //* 2^32
    }
    void opcodetoctrl()
    {
        bool calc[4] = {R.opcode == 000000, R.opcode == 100011, R.opcode == 101011, R.opcode == 000100};
        R.Ctrl.RegDst = (calc[0]);
        R.Ctrl.ALUSrc = R.Ctrl.MemRead = (calc[1] | calc[2]);
        R.Ctrl.MemToReg = (calc[1]);
        R.Ctrl.RegWrite = (calc[0] | calc[1]);
        R.Ctrl.MemWrite = (calc[2]);
        R.Ctrl.Branch = R.Ctrl.ALUOp[1] = calc[3];
        R.Ctrl.ALUOp[0] = calc[0];
    }
    void signextend()
    {
        R.Imm_se = (R.Imm[15]) ? R.Imm.to_ulong() : R.Imm.to_ulong();
    }
    void controlCalculations()
    {
        //*Processing instruction memory to parts
        R.opcode = InstructionMem.to_ulong() >> 26;
        opcodetoctrl();
        R.Rs = InstructionMem.to_ulong() >> 21;
        R.Rt = InstructionMem.to_ulong() >> 16;
        R.Write_Reg = InstructionMem.to_ulong() >> 11;
        R.Shamt = (R.Ctrl.RegDst) ? InstructionMem.to_ulong() >> 6 : R.Rt;
        R.Imm = InstructionMem.to_ulong();
        R.ALUControl = InstructionMem.to_ulong();
        //*calculating jump value (only used if jump is enabled)
        R.ALUOut = (R.Imm[15]) ? 4294901760 + R.Imm.to_ulong() : R.Imm.to_ulong(); //* if negative, 4294901760 = 11111111111111110000000000000000
        R.Read_Data1 = registers[R.Rs.to_ulong()];                                 //*Read from registers for Rs and Rt
        R.Read_Data2 = registers[R.Rt.to_ulong()];
        signextend();
        /*
        switch (opcode.to_ulong())
        {
        case rtype: //* R-type with opcode 000000
            Ctrl.ALUOp = 2;
            Ctrl.RegDst = Ctrl.RegWrite = 1;
            Ctrl.ALUSrc = Ctrl.MemToReg = Ctrl.MemRead = Ctrl.MemWrite = Ctrl.Branch = 0;
            switch (funct.to_ulong())
            {
            case add:

                break;
            case sub:
                break;
            case jr:
                break;
            default:
                break;
            }
            break;
        case addi: //* addi with opcode 001000
            break;
        case beq: //* beq with opcode 001000
            Ctrl.ALUOp = 1;
            break;
        case j: //* j with opcode 001000
            break;
        case lw:
        case sw:
            Ctrl.ALUOp = 0;
            break;
        default:
            break;
        }
        */
    }

private:
    unordered_map<string, bitset<32>> bitfields;
    bitset<32> InstructionMem;
    enum functdict
    {
        sll = 0,
        srl = 2,
        sra = 3,
        silv = 4,
        srlv = 6,
        srav = 7,
        jr = 8,
        jalr = 9,
        mult = 24,
        multu = 25,
        div = 26,
        divu = 27,
        add = 32,
        addu = 33,
        sub = 34,
        subu = 35,
        andd = 36,
        orr = 37,
        xorr = 38,
        norr = 49,
        slt = 42,
        sltu = 43
    };
    enum opcodedict
    {
        rtype = 0,
        j = 2,
        jal = 3,
        beq = 4,
        bne = 5,
        addi = 8,
        addiu = 9,
        slti = 10,
        sltiu = 11,
        andi = 12,
        ori = 13,
        xori = 14,
        lui = 15,
        lw = 35,
        sw = 43
    };
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
        ofstream file("Outputs/OutputTest.txt", std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            file << "~~~~~~~~~~\n|Loop: " << loop << " |\n~~~~~~~~~~\n";
            file << "~opSTAGES~\n~|IF |ID | E |MS |WB |~" << endl;
            file << "~| " << F.op << " | " << D.op << " | " << E.op << " | " << M.op << " | " << W.op << " |~" << endl;
        }
        else
            cout << "\nOutputs/OutputTest.txt failed to open!";
        file.close();
        switch (debug)
        {
        default:
        case 1:
            F.print();
            break;
        case 2:
            D.print("Decode");
            break;
        case 3:
            E.print("Execute");
            break;
        case 4:
            M.print("Nemory Access");
            break;
        case 5:
            W.print("Write Back");
            break;
        case 6:
            F.print();
            D.print("Decode");
            E.print("Execute");
            M.print("Nemory Access");
            W.print("Write Back");
            break;
        }
        ofstream file2("Outputs/OutputReg.txt", std::ios_base::app);
        if (file2.is_open())
        {
            file2 << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            file2 << "~~~~~~~~~~\n|Loop: " << loop << " |\n~~~~~~~~~~\n";
            for (auto i : registers)
                file2 << i << endl;
            file2 << endl;
        }
        else
            cout << "\nOutputs/OutputReg.txt failed to open!";
        file2.close();
    }

private:
protected:
};
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
            while (getline(imem >> ws, str))
            {
                if (str.find(":") != string::npos)
                {
                    str = str.substr(0, str.find(":", 0));
                    cout << (insmemstart + 4 * counter) << " ~ " << str << endl;
                    pointer[str] = insmemstart + 4 * counter; //* 4194304 = (hex)0x00400000
                }
                else
                    counter++;
            }
            cout << "~~~~~~~~~~~~~~~~~~~~~" << endl;
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
            cout << "~InstructionMemory~~~~~~~~" << endl;
            while (getline(imem >> ws, str))
            {
                str = str.substr(0, str.find("#", 0));
                if (str.size() && str.find(":") == string::npos)
                {
                    for (char i : illegalvalues)
                        str.erase(remove(str.begin(), str.end(), i), str.end());
                    mem.push_back(str);
                    cout << insmemstart + counter * 4 << ": " << str << endl;
                    counter++;
                }
            }
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
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
                    t_Rstd.push_back(registerlib[word]);
                }
                buffer << t_Rstd[1] << t_Rstd[2] << t_Rstd[0] << bitset<5>(0);
                if (!funct.to_ulong())
                    funct = 34;
                buffer << funct;
                break;
            case addi:
            case beq:
                for (int i = 0; i < 2; i++)
                {
                    iss >> word;
                    t_Rstd.push_back(registerlib[word]);
                }
                buffer << t_Rstd[1] << t_Rstd[0];
                iss >> word;
                if (cases == addi)
                    immediate = stoi(word);
                else
                    immediate = pointer[word].to_ulong() - (insmemstart + 4 * counter);
                buffer << immediate;
                break;
            case lw:
            case sw:
                iss >> word;
                t_Rstd.push_back(registerlib[word]);
                iss >> word;
                buffer << bitset<5>(registerlib[word.substr(word.find("(", 0) + 1, word.find(")", 0) - word.find("(", 0) - 1)]) << t_Rstd[0] << bitset<16>(stoi(word.substr(0, word.find("(", 0))));
                break;
            case j:
                iss >> word;
                buffer << bitset<26>(pointer[word].to_ulong() >> 2);
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
        membin.push_back(bitset<32>(4294967295));
        cout << "\n\x1B[94mInstruction Memory Encoded\033[0m" << endl;
        cout << "~~~~InstructionMemoryBinary~~~~~~~~~~~~~~" << endl;
        for (int counter = 0; counter < membin.size(); counter++)
        {
            cout << insmemstart + counter * 4 << ": " << membin[counter] << endl;
        }

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    }
    vector<bitset<32>> outMem() { return membin; }

private:
    vector<char> illegalvalues = {',', '\t'};
    vector<bitset<32>> membin;
    vector<string> mem;
    enum functionsenum
    {
        add,
        addi,
        sub,
        j,
        jr,
        beq,
        sw,
        lw
    };
    unordered_map<string, int> functions = {{"add", 0}, {"addi", 1}, {"sub", 2}, {"j", 3}, {"jr", 4}, {"beq", 5}, {"sw", 6}, {"lw", 7}};
    unordered_map<string, int> registerlib = {{"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}};
    vector<bitset<6>> opcodelib = {0, 8, 0, 2, 0, 4, 43, 35};
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
/************************ MAIN ************************/
int main()
{
    ClearFiles();
    //! Create State and Initialize
    stateClass state, nextState;
    registers.resize(32);
    int loop = 0;
    bool op = 1;
    InstructionMem IMEM;
    DataMem DMEM;
    state.print(0);
    //! MainCycle

    cout << "\x1B[94mMain Cycle Initialized\033[0m" << endl;
    while (op)
    {
        cout << state.F.PCOut().to_ulong() << ": " << state.F.op << state.D.op << state.E.op << state.M.op << state.W.op << endl;
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
            nextState.D = state.D;
            nextState.D.controlCalculations();
            nextState.E.inReg(state.D.outReg());
        }
        // Fetch
        nextState.D.op = state.F.op;
        if (state.F.op)
        {
            nextState.D.readInsMem((state.F.PCOut().to_ulong() - insmemstart) / 4, IMEM.outMem());
            if (nextState.D.isEnd())
            {
                nextState.F.op = nextState.D.op = 0;
            }
            else
                nextState.F.countUp(4);
        }

        loop++;
        state = nextState;
        state.print(loop);

        if (!state.F.op && !state.D.op && !state.E.op && !state.M.op && !state.W.op)
            op = 0;
    }
    return 0;
}

void ClearFiles()
{
    remove("Outputs/OutputTest.txt");
    remove("Outputs/OutputReg.txt");
}
