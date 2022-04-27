#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;

#define memorysize 1000
#define insmemstart 4194304
#define datamemstart 268500992
/************************  FILE NAMES ********************************/
string imemfile = "imem.s";
string datamemout = "Outputs/DataMemory.txt";
string datamemfile = "DataMemory.txt";
/************************  GLOBAL VARIABLES **************************/
vector<bitset<32>> datamem;
unordered_map<string, bitset<32>> pointer;

/************************ FUNCTIONS & CLASSES ************************/

void ClearFiles();
void ClearDataMemory();

class InstructionMem
{
public:
    InstructionMem(); // TODO change to read words

    void readInstructions();
    void InsMemConv();
    vector<bitset<32>> outMem();

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
        lw,
        sll,
        nop
    };
    unordered_map<string, int> functions = {{"add", 0}, {"addi", 1}, {"sub", 2}, {"j", 3}, {"jr", 4}, {"beq", 5}, {"sw", 6}, {"lw", 7}, {"sll", 8}};
    unordered_map<string, int> registerlib = {{"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}};
    vector<bitset<6>> opcodelib = {0, 8, 0, 2, 0, 4, 43, 35, 0};
    unordered_map<string, int> functlib = {{"add", 32}, {"sub", 34}, {"sll", 0}, {"nop", 0}};
};
class DataMem
{
public:
    DataMem();
    void DataMemOut();

private:
};
