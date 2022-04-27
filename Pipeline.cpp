#include <iostream>
#include "MIPSAssembler.cpp"
using namespace std;
// https://stackoverflow.com/questions/59474537/code-runner-configuration-for-running-multiple-cpp-classes-in-vscode
// for fixing multiple files

#define eoim 4294967295
bool mode = 1;    //* 0-binary mode, 1-assembly mode
int debug = 1;    //* Enables Debug push
int slowmode = 0; //* Enables Slow Mode for debugging
string RED = "\x1B[91m\x1B[40m";
string DEF = "\x1B[37m\x1B[40m";
/************************  FILE NAMES ********************************/

string registerout = "Outputs/OutputReg.txt";
string debugout = "Outputs/DebugOutput.txt";

/************************  GLOBAL VARIABLES **************************/
bitset<32> PCGlobal; //* Program Counter
vector<bitset<32>> registers;
/************************ AVAILABLE COMMANDS *************************/
//*       add,addi,sub,j,jr,beq,sw,lw,nop
/************************ FUNCTIONS & CLASSES ************************/

class IF
{ //! Fetch Class
public:
    IF() { PCGlobal = insmemstart; }
    bool op = 1; //* Operational state
    void countUp(int inc)
    {
        PCGlobal = PCGlobal.to_ulong() + inc;
    }
    void print()
    {
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~Fetch Stage~~~~" << endl;
            file << "~PC dec      : " << PCGlobal.to_ulong() << endl;
            file << "~PC bin      : " << PCGlobal << endl
                 << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
        file.close();
    }
    bitset<32> PCOut()
    {
        return PCGlobal;
    }

private:
};
struct IDControl
{ //? Control Structure to read opcode
    bool RegDst, Jump, Branch, MemRead, MemToReg, MemWrite, ALUSrc, RegWrite;
    bitset<2> ALUOp;
};
struct RegStruct
{
    IDControl Ctrl;
    bool zeroflag = 0;
    bitset<6> opcode = 0, funct = 0;
    bitset<4> ALUControl = 0;
    bitset<5> Rs = 0, Rt = 0, Write_Reg = 0, Shamt = 0;
    bitset<16> Imm = 0;
    bitset<26> JumpAddress = 0;
    bitset<32> Read_Data1 = 0, Read_Data2 = 0, ALUOut = 0, Imm_se = 0, AddALU = 0, PC = 0, DMEM_Read_Data = 0;
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
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << endl
                 << "~~~~" << stage << " Stage~~~~" << endl;
            file << "~Opcode      : " << R.opcode << endl;
            file << "~PC          : " << R.PC.to_ulong() << endl;
            file << "~Rs          : " << R.Rs << endl;
            file << "~Rt          : " << R.Rt << endl;
            file << "~Imm         : " << R.Imm << endl;
            file << "~Write_Reg   : " << R.Write_Reg << endl;
            file << "~Read_Data1  : " << R.Read_Data1 << endl;
            file << "~Read_Data2  : " << R.Read_Data2 << endl;
            file << "~Control Data: " << R.Ctrl.RegDst << " " << R.Ctrl.Jump << " " << R.Ctrl.Branch << " " << R.Ctrl.MemRead << " " << R.Ctrl.MemToReg << " " << R.Ctrl.ALUOp << " " << R.Ctrl.MemWrite << " " << R.Ctrl.ALUSrc << " " << R.Ctrl.RegWrite << "~" << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
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
{ //! Decode Class=
public:
    void readInsMem(bitset<32> readAddress, vector<bitset<32>> mem)
    {
        InstructionMem = bitset<32>(mem[readAddress.to_ulong()]);
        R.PC = (readAddress.to_ulong() * 4 + insmemstart) + 4;
    }
    void clearInsMem()
    {
        InstructionMem = 0;
    }
    bool isEnd()
    {
        return (InstructionMem == bitset<32>(eoim)); //* 2^32
    }
    void opcodetoctrl()
    {
        bool calc[4] = {R.opcode == rtype, R.opcode == lw, R.opcode == sw, R.opcode == beq}; //*000000 100011 101011 000100
        R.Ctrl.RegDst = (calc[0]);
        R.Ctrl.ALUSrc = (calc[1] | calc[2]);
        R.Ctrl.MemToReg = R.Ctrl.MemRead = (calc[1]);
        R.Ctrl.RegWrite = (calc[0] | calc[1]);
        R.Ctrl.MemWrite = (calc[2]);
        R.Ctrl.Branch = R.Ctrl.ALUOp[0] = calc[3];
        R.Ctrl.ALUOp[1] = calc[0];
        if (R.opcode == j)
            R.Ctrl.Jump = 1;
        if (R.opcode == addi)
        {
            R.Ctrl.ALUSrc = 1;
            R.Ctrl.RegWrite = 1;
        }
    }
    void signextend()
    {
        R.Imm_se = (R.Imm[15]) ? (65535 << 16) + R.Imm.to_ulong() : R.Imm.to_ulong();
    }
    void controlCalculations()
    {
        //*Processing instruction memory to parts
        R.opcode = InstructionMem.to_ulong() >> 26;
        R.funct = InstructionMem.to_ulong();
        opcodetoctrl();
        R.Rs = InstructionMem.to_ulong() >> 21;
        R.Rt = InstructionMem.to_ulong() >> 16;
        R.Write_Reg = (R.Ctrl.RegDst) ? InstructionMem.to_ulong() >> 11 : R.Rt;
        R.Shamt = InstructionMem.to_ulong() >> 6;
        R.Imm = InstructionMem.to_ulong();
        R.ALUControl = InstructionMem.to_ulong();
        //*calculating jump value (only used if jump is enabled)
        R.ALUOut = (R.Imm[15]) ? 4294901760 + R.Imm.to_ulong() : R.Imm.to_ulong(); //* if negative, 4294901760 = 11111111111111110000000000000000
        R.Read_Data1 = registers[R.Rs.to_ulong()];                                 //*Read from registers for Rs and Rt
        R.Read_Data2 = registers[R.Rt.to_ulong()];
        // cout << RED << "Read Datas from: " << R.Rs.to_ulong() << " and " << R.Rt.to_ulong() << endl;
        // cout << "Datas: " << R.Read_Data1 << " : " << R.Read_Data2 << DEF << endl;
        R.JumpAddress = InstructionMem.to_ulong();
        signextend();
        PCGlobal = (R.Ctrl.Jump) ? (R.JumpAddress.to_ulong() << 2) : PCGlobal;
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
    void print(string stage)
    {
        BaseID::print(stage);
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "|---------------------------------------------------|" << endl
                 << "|ALUControl Output: " << op << "                               |" << endl
                 << "|MUX for Main ALU : " << ALUInputMux << "|" << endl
                 << "|Main ALU OUtput  : " << R.ALUOut << "|" << endl
                 << "|ZeroFlag         : " << R.zeroflag << "                               |" << endl
                 << "|Add ALU Output   : " << R.AddALU << "|" << endl
                 << "|---------------------------------------------------|" << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
        file.close();
    };
    bitset<3> ALUControl()
    {
        bitset<4> func = (R.funct.to_ulong());
        outp[0] = (R.Ctrl.ALUOp[1] & (func[0] | func[3]));         //* op3 - Rightmostvalue
        outp[1] = !(R.Ctrl.ALUOp[1] & func[2]);                    //* op2
        outp[2] = (R.Ctrl.ALUOp[0] | (R.Ctrl.ALUOp[1] & func[1])); //* op1
        return outp;
    }
    void ALU()
    {
        ALUInputMux = (R.Ctrl.ALUSrc) ? R.Imm_se : R.Read_Data2;
        switch (ALUControl().to_ulong())
        {
        case 0: //* and function
            R.ALUOut = R.Read_Data1.to_ulong() & ALUInputMux.to_ulong();
            break;
        case 1: //* or function
            R.ALUOut = R.Read_Data1.to_ulong() | ALUInputMux.to_ulong();
            break;
        case 2: //* add function
            R.ALUOut = R.Read_Data1.to_ulong() + ALUInputMux.to_ulong();
            break;
        case 6: //* sub function
            R.ALUOut = R.Read_Data1.to_ulong() - ALUInputMux.to_ulong();
            break;
        case 7: //* slt function
            R.ALUOut = (R.Read_Data1.to_ulong() < ALUInputMux.to_ulong()) ? 1 : 0;
            break;
        case 12: //* nor function
            R.ALUOut = !(R.Read_Data1.to_ulong() | ALUInputMux.to_ulong());
            break;
        }
        R.zeroflag = (R.ALUOut == 0) ? 1 : 0;
        // if (R.opcode == 43)
        //     cout << R.Imm_se.to_ulong() << ":" << R.Read_Data2.to_ulong() << endl
        //          << R.ALUOut.to_ulong() << "=" << R.Read_Data1.to_ulong() << "+" << ALUInputMux.to_ulong() << endl;
        AddALU();
        PCGlobal = (R.zeroflag & R.Ctrl.Branch) ? R.AddALU : PCGlobal;
    }
    void AddALU()
    {
        R.AddALU = R.PC.to_ulong() + (R.Imm_se.to_ulong() - 8);
        // cout << R.AddALU.to_ulong() << endl;
    }

private:
    bitset<3> outp;
    bitset<32> ALUInputMux;
};
class MEM : public BaseID
{ //! Memory Access Class
public:
    void print(string stage)
    {
        BaseID::print(stage);
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "|-----------------------------------------------|" << endl
                 << "|MemWrite     : " << R.Ctrl.MemWrite << "                               |" << endl
                 << "|MemRead      : " << R.Ctrl.MemRead << "                               |" << endl;
            file << "|-----------------------------------------------|" << endl
                 << "|Branching: " << (R.zeroflag & R.Ctrl.Branch) << "                                   |" << endl
                 << "|Jumping: " << R.Ctrl.Jump << "                                     |" << endl;
            if (R.Ctrl.Jump)
                file << "|Jump Address:" << R.JumpAddress << "                               |" << endl;
            if (R.Ctrl.MemWrite)
                file << "|Data Location: " << (R.ALUOut.to_ulong() - insmemstart) / 4 << "                               |" << endl
                     << "|Data Stored  : " << datamem[(R.ALUOut.to_ulong() - insmemstart) / 4] << "                               |" << endl;
            file << "|-----------------------------------------------|" << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
        file.close();
    };
    void DMEM() //* INPUT Address from ALU, WriteData from ReadData2, OUTPUT ReadData
    {
        if (R.Ctrl.MemRead)
            R.DMEM_Read_Data = datamem[(R.ALUOut.to_ulong() - insmemstart) / 4];
        if (R.Ctrl.MemWrite)
            datamem[(R.ALUOut.to_ulong()) / 4] = R.Read_Data2;
    }
    bitset<32> PCOut()
    {
        return PCTemp;
    }

private:
    bitset<32> PCTemp;
};
class WB : public BaseID
{ //! Write Back Class
public:
    void print(string stage)
    {
        BaseID::print(stage);
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "|----------------------------------------------|" << endl
                 << "|Mem To Reg  : " << R.Ctrl.MemToReg << "                               |" << endl
                 << "|Reg Stored  : " << registers[R.Write_Reg.to_ulong()] << "|" << endl;
            if (R.Ctrl.MemToReg)
                file << "|Reg Location: " << R.Write_Reg.to_ulong() << "                               |" << endl;
            file << "|----------------------------------------------|" << endl
                 << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
        file.close();
    };
    void MtoReg()
    {
        if (R.Ctrl.RegWrite)
        {
            registers[R.Write_Reg.to_ulong()] = (R.Ctrl.MemToReg) ? R.DMEM_Read_Data : R.ALUOut;
            cout << registers[5].to_ulong() << endl;
        }
    }

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
        ofstream file(debugout, std::ios_base::app); // https://www.delftstack.com/howto/cpp/how-to-append-text-to-a-file-in-cpp/
        if (file.is_open())
        {
            file << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            file << "~~~~~~~~~~\n|Loop: " << loop << " |\n~~~~~~~~~~\n";
            file << "~opSTAGES~\n~|IF |ID | E |MS |WB |~" << endl;
            file << "~| " << F.op << " | " << D.op << " | " << E.op << " | " << M.op << " | " << W.op << " |~" << endl;
        }
        else
            cout << "\n"
                 << debugout << " failed to open!";
        file.close();
        switch (debug)
        {
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
        default:
            F.print();
            D.print("Decode");
            E.print("Execute");
            M.print("Nemory Access");
            W.print("Write Back");
            break;
        }
    }
    template <typename T>
    void printregisters(T loop)
    {
        ofstream file2(registerout); //, std::ios_base::app
        if (file2.is_open())
        {
            file2 << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            file2 << "~~~~~~~~~~\n|Loop: " << loop << " |\n~~~~~~~~~~\n";
            for (int i = 0; i < 32; i++)
            { // setfill('0') << setw(8) << hex <<
                file2 << setfill(' ') << setw(4) << register_pos[i] << ": " << registers[i].to_ulong() << endl;
            }
            file2 << endl;
        }
        else
            cout << "\n"
                 << registerout << " failed to open!";
        file2.close();
    }

private:
    vector<string> register_pos = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"};

protected:
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
    ClearDataMemory();
    //! Create State and Initialize
    registers.resize(32);
    fill(registers.begin(), registers.end(), 0);
    stateClass state, nextState;
    int loop = 0;
    bool op = 1;
    InstructionMem IMEM;
    DataMem DMEM;
    //! MainCycle
    // state.print(loop);
    cout << "\x1B[94mMain Cycle Initialized\033[0m" << endl;
    while (op)
    {
        nextState.D.readInsMem((state.F.PCOut().to_ulong() - insmemstart) / 4, IMEM.outMem());
        if (nextState.D.isEnd())
        {
            nextState.F.op = 0;
            nextState.D.clearInsMem();
        }
        // cout << state.F.PCOut().to_ulong() << ": " << state.F.op << state.D.op << state.E.op << state.M.op << state.W.op << endl;
        // WriteBack
        if (state.W.op)
        {
            state.W.MtoReg();
        }
        // cout << "Running Memory\n";
        // MemoryAccess
        nextState.W.op = state.M.op;
        if (state.M.op == 1)
        {
            state.M.DMEM();
            nextState.W.inReg(state.M.outReg());
        }
        // cout << "Running Execute\n";
        // Execute
        nextState.M.op = state.E.op;
        if (state.E.op)
        {
            state.E.ALU();
            nextState.M.inReg(state.E.outReg());
        }
        // cout << "Running Decode\n";
        // Decode
        nextState.E.op = state.D.op;
        if (state.D.op)
        {
            state.D.controlCalculations();
            nextState.E.inReg(state.D.outReg());
        }
        // cout << "Running Fetch\n";
        // Fetch
        nextState.D.op = state.F.op;
        if (state.F.op)
        {
            nextState.D.readInsMem((state.F.PCOut().to_ulong() - insmemstart) / 4, IMEM.outMem());
            nextState.F.countUp(4);
        }
        loop++;
        // state.print(loop);
        // state.printregisters(loop);
        state = nextState;
        // cout << loop << ": " << PCGlobal.to_ulong() << endl;

        if (!state.F.op & !state.D.op & !state.E.op & !state.M.op & !state.W.op)
            op = 0;
        if (slowmode)
            system("pause");
    }
    loop++;
    state.W.op = 0;
    // state.print(loop);
    state.printregisters("Final");
    DMEM.DataMemOut();
    cout << "\x1B[94mCode Ran Fully with No Errors\033[0m" << endl;
    return 0;
}
