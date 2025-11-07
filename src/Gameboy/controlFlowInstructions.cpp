#include "gameboy.h"

void Gameboy::callFunction() {
    unsigned char lsb = mem[PC];
    PC++;

    unsigned char msb = mem[PC];
    PC++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    unsigned char msbPC = (PC >> 8);
    unsigned char lsbPC = PC & 0x00FF;

    SP--;
    mem[SP] = msbPC;
    SP--;
    mem[SP] = lsbPC;

    if (LOGGING) printf("CALL FUNCTION: SET PC TO ADDR 0x%04x\n", addr);

    PC = addr;
}

void Gameboy::ret() {
    unsigned char lsb = mem[SP];
    SP++;

    unsigned char msb = mem[SP];
    SP++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    PC = addr;

    if (LOGGING) printf("RETURN: SET PC TO ADDR 0x%04x\n", addr);
}
