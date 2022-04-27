#include <iostream>
#include <bitset>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
using namespace std;

unordered_map<string, bitset<32>> pointer;
unordered_map<string, bitset<32>> functions = {{"add", 0}, {"addi", 1}, {"sub", 2}, {"j", 3}, {"jr", 4}, {"beq", 5}};
unordered_map<string, int> registerlib = {{"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}};

void stringbyword()
{
    string str = "add $t0, $t1, $a0";
    istringstream iss(str);
    string word;
    while (iss >> word)
    {
        word.erase(std::remove(word.begin(), word.end(), ','), word.end());
        cout << word << endl;
    }
}

void opchain()
{
    bitset<6> opcode, funct;
    bitset<5> Rs, Rt, Rd, shamt;
    opcode = bitset<6>(5);
    funct = bitset<6>(10);
    Rs = bitset<5>(11);
    Rt = bitset<5>(12);
    Rd = bitset<5>(13);
    shamt = bitset<5>(14);
    bitset<32> final(opcode.to_ulong() << 26 | Rs.to_ulong() << 21 | Rt.to_ulong() << 16 | Rd.to_ulong() << 11 | shamt.to_ulong() << 6 | funct.to_ulong());
    cout << opcode << endl
         << Rs << endl
         << Rt << endl
         << Rd << endl
         << shamt << endl
         << funct << endl
         << endl;
    cout << final << endl;
}

void unorderedmaptest()
{
    unordered_map<string, int> functions = {{"add", 0}, {"addi", 1}, {"sub", 2}, {"j", 3}, {"jr", 4}, {"beq", 5}};
    cout << functions["addi"];
}

void readpointer()
{
    ifstream imem("imem.mip");
    string str;
    int counter = 0;
    if (imem.is_open())
    {
        while (getline(imem, str))
        {
            if (str.find(":") != string::npos)
            {
                cout << counter << " - " << str << endl;
                str = str.substr(0, str.find(":", 0));
                pointer[str] = 4194304 + counter * 4; //* 4194304 = (hex)0x00400000
            }
            else
                counter++;
        }
    }
    cout << endl
         << endl;
    for (auto x : pointer)
        cout << hex << x.second.to_ulong() << ": ~ " << x.first << endl;
    cout << endl
         << endl
         << endl
         << hex
         << pointer["gae"].to_ulong();
}

void bitshifting()
{

    system("cls");
    bitset<8> value = 142; // 10001110
    bitset<4> final = value.to_ulong() >> 2;
    cout << value << endl
         << "  " << final << endl;
}

void squares()
{
    int storage[200];
    int a2 = 0;                        //*Addi
    storage[0] = a2;                   //*sw
    for (int a1 = 1; a1 < 401; a1 + 2) //*beq
    {
        a2 += a1;             //*add
        storage[a1 / 2] = a2; //*sw
    }
}

void squares2()
{
    int storage[200];
    int a1 = 1;                      //*Addi
    int a2 = 0;                      //*Addi
    storage[0] = a2;                 //*sw
    for (int a3 = 1; a3 < 201; a3++) //*beq
    {
        cout << a1 << ":" << a2 << endl;
        a2 += a1;         //*add
        storage[a3] = a2; //*sw
        a1 = a1 + 2;      //*Addi
    }
    cout << a1 - 1 << ":" << a2 << endl;
}

vector<string> mem;
int insmemstart = 4194304;
void instructionmemupdate()
{
    vector<char> illegalvalues = {',', '\t'};
    unordered_set<string> twolinesback;
    unordered_set<string> onelineback;
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

int main()
{
    system("cls");
    instructionmemupdate();
    cout << "\n\x1B[94mInstruction Memory Loaded\033[0m" << endl;
    cout << "~InstructionMemory~~~~~~~~" << endl;
    for (int i = 0; i < mem.size(); i++)
        cout << insmemstart + i * 4 << ": " << mem[i] << endl;
    cout << "POINTERS" << endl
         << endl;
    for (auto const &pair : pointer)
    {
        cout << pair.second.to_ulong() << ": " << pair.first << endl;
    }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    return 0;
}
