#include "MIPSAssembler.h"

/************************ FUNCTIONS & CLASSES ************************/

void ClearFiles()
{
    remove("Outputs/DataMemory.txt");
    remove("Outputs/DebugOutput.txt");
    remove("Outputs/OutputReg.txt");
}

void ClearDataMemory()
{
    remove("DataMemory.txt");
}

/***InstructionMem Class Start***/
InstructionMem::InstructionMem()
{
    readInstructions();
    InsMemConv();
}

void InstructionMem::readInstructions()
{
    vector<char> illegalvalues = {',', '\t'};
    string imemfile = "imem.s";
    ifstream imem(imemfile);
    string str, tempstr;
    int counter = 0;
    if (imem.is_open())
    {
        while (getline(imem >> ws, str))
        {
            str = str.substr(0, str.find("#", 0));
            cout << str << endl;
            if (str.size() && str.find(":") == string::npos)
            {
                for (char i : illegalvalues)
                    str.erase(remove(str.begin(), str.end(), i), str.end());
                mem.push_back(str);
                if ((str.substr(0, 1) == "j") || (str.substr(0, 3) == "beq"))
                {
                    mem.push_back("nop");
                    counter++;
                }
            }
            else if (str.find(":") != string::npos)
            {
                str = str.substr(0, str.find(":", 0));
                pointer[str] = insmemstart + 4 * counter; //* 4194304 = (hex)0x00400000
                mem.push_back("nop");
            }
            counter++;
        }
    }
    else
        cout << "\n\x1B[91mInstruction Memory Load failed\033[0m" << endl;
}

void InstructionMem::InsMemConv()
{
    for (int counter = 0; counter < mem.size(); counter++)
    {
        stringstream buffer;
        string word;
        bitset<6> funct = 0;
        bitset<16> immediate;
        vector<bitset<5>> t_Rstd;
        string tempstr[3] = {" "};
        stringstream iss(mem[counter]);
        iss >> word;
        buffer << opcodelib[functions[word]];
        int cases = functions[word];
        switch (cases)
        {
        case nop:
            buffer << bitset<26>(0);
            break;
        case add:
        case sub:
            funct = functlib[word];
            for (int i = 0; i < 3; i++)
            {
                iss >> word;
                t_Rstd.push_back(registerlib[word]);
            }
            buffer << t_Rstd[1] << t_Rstd[2] << t_Rstd[0] << bitset<5>(0);
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

vector<bitset<32>> InstructionMem::outMem() { return membin; }
/***InstructionMem Class End***/

/***DataMem Class Start***/
DataMem::DataMem()
{
    datamem.resize(memorysize);
    ifstream dmem(datamemfile);
    string str;
    int i = 0;
    if (dmem.is_open())
    {
        cout << "\x1B[94mData Memory Loaded\033[0m" << endl
             << endl;
        while (getline(dmem, str))
        {
            datamem[i] = bitset<32>(str);
            i++;
        }
    }
    else
        cout << "\x1B[91mData Memory Load failed\033[0m" << endl
             << endl;
}

void DataMem::DataMemOut()
{
    ofstream file(datamemfile);
    if (file.is_open())
    {
        for (int i = 0; i < datamem.size(); i++)
        {
            file << datamem[i] << endl;
        }
        file << endl;
    }
    else
        cout << "\n"
             << datamemfile << " failed to open!";
    file.close();
    ofstream file2(datamemout);
    if (file2.is_open())
    {
        for (int i = 0; i < datamem.size(); i++)
        {
            file2 << setfill('0') << setw(4) << i << ": " << datamem[i].to_ulong() << endl;
        }
        file2 << endl;
    }
    else
        cout << "\n"
             << datamemout << " failed to open!";
    file2.close();
}
/***DataMem Class End***/