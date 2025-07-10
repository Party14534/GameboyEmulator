#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <string>
#include <vector>
#include <stdio.h>
#include <cstdlib>

enum RegisterIndex {
    A = 0, B, C, D, E, F, H, L
};

struct Registers {
    std::vector<unsigned char> registers;

    // Flags register
    bool zero = false;
    bool subtract = false;
    bool halfCarry = false;
    bool carry = false;

    void setAF(short unsigned int val);
    void setBC(short unsigned int val);
    void setDE(short unsigned int val);
    void setHL(short unsigned int val);

    unsigned char fRtoU8();
    void u8toFR(unsigned char byte);
};

struct Gameboy {
    // Register file
    short unsigned int PC = 0; // Program counter
    short unsigned int SP = 0; // Stack pointer
    short unsigned int IX = 0; // Index register
    short unsigned int IY = 0; // Index register
    unsigned char A = 0; // 8-bit accumulator
    unsigned char I = 0; // Interrupt Page Address register
    unsigned char R = 0; // Memory Refresh register
    unsigned char IE = 0; // Interrupt Enable
    Registers r;

    // Emulator variables
    std::string romPath;
    std::vector<unsigned char> mem;

    /*
     * Function Definitions
    */
    Gameboy(std::string _romPath);
    void FDE();
    unsigned char fetch();
    void decode(unsigned char instruction);

    // Instructions
    void Add(RegisterIndex target, bool carry);
    void AddFromMemory(bool carry);
};

#endif
