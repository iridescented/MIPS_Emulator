#include <iostream>
#include <string>
#include <bitset>
#include <bits/stdc++.h>

using namespace std;

int address[32];

class instuctionFetch
{
    public:
        int programCounter[32] (int &address, int count)      // Input indicates address of next instruction
        {
            count += 1;     
            int nextAddress[32] = address;                          // Program Counter is 32 bits
            return nextAddress;                                         // Output points to current instruction
        }

        int adder1 (int variable)                                        // ADD function in IF 
        {
            int answer = variable + 4 ;                                 //! NOT SURE HOW TO IMPLEMENT THIS ADD            
            return answer;
        }

        int instructionMemory[32] (int address[32])               // 32 bits instruction address input
        {
            bitset<32> data = &address;                                 // Taking data from address
            return data;                                                // Outputs data from that address
        }
};

class instructionDecode : public instructionFetch
{
    uint32_t convert16to32bit (const uint16_t bits16)                   // sign extend from 16 to 32 bits
    {
        uint32_t bits32 = (uint32_t) bits16;
        return bits32;
    }

    int registerFile [32][32] ( int read1[5], int read2[5], int writeRegister[5], int writeData[32], bool writeEnable )
    {
        int data1[32] = &read1;
        int data2[32] = &read2;

        if (writeEnable)
        {
            writeData = &writeRegister;
        }

        return [data1][data2]
    }
};

class execute : public instructionDecode, instructionFetch
{
    public:
        int adder2 ( int bits32 )
        {
            bitset<32> shiftedLeft = bits32 << 2;
            add = fromAdder + shiftedLeft                                     //! first variable should come from adder 1 but dk how
        }

        int ALU                                                             //! no idea what should be in ALU
};

class memory : public instructionDecode, instructionFetch, execute
{
    public:
        int dataMemory[32] ( int &address, int writeData[32], int readData[32], bool writeEnable )
        {

            return address = writeEnable ? writeData : readData;        
            // if (writeEnable)
            //     address = writeData;                                     //? If 1, writes writeData onton Address

            // else
            //     address = readData;                                      //? If 0, reads Address onto readData
        }
};

int main()
{
    return 0;
}