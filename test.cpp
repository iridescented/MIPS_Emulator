#include <iostream>
#include <bitset>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

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

int main()
{
    system("cls");
    stringbyword();
    return 0;
}