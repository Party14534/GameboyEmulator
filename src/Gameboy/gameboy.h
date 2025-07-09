#ifndef GAMEBOY_H
#define GAMEBOY_H

struct Registers {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char e;
    unsigned char f;
    unsigned char h;
    unsigned char l;

    // Flags register
    bool zero;
    bool subtract;
    bool halfCarry;
    bool carry;

    void setAF(short unsigned int val);
    void setBC(short unsigned int val);
    void setDE(short unsigned int val);
    void setHL(short unsigned int val);

    unsigned char fRtoU8();
    void u8toFR(unsigned char byte);
};

struct Gameboy {
    // Register file
    short unsigned int PC; // Program counter
    short unsigned int SP; // Stack pointer
    unsigned char A; // 8-bit accumulator
    Registers r;

    unsigned char IR; // Instruction register
    unsigned char IE; // Interrupt Enable
};

#endif
