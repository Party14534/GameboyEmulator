#include "gameboy.h"

void Gameboy::load(RegisterIndex target, RegisterIndex value) {
    r.registers[target] = r.registers[value];
}

// Two cycles
void Gameboy::loadFromMemory(RegisterIndex target) {
    unsigned short int addr = r.registers[RegisterIndex::L];
    
    addr += (r.registers[RegisterIndex::H] << 8);
    unsigned char value = mem[addr];

    r.registers[target] = value;
}

// Two cycles
// 0x70 - 0x75, 0x77
void Gameboy::loadToMemory(RegisterIndex value) {
    unsigned short int addr = r.getHL();

    mem[addr] = r.registers[value];
}

// Two cycles
void Gameboy::loadToRegister(RegisterIndex target) {
    unsigned char data = mem[PC];
    PC++;
    
    r.registers[target] = data;
}

// 3 cycles
void Gameboy::loadImmediateDataToMemory() {
    unsigned char data = mem[PC];
    PC++;

    mem[r.getHL()] = data;
}

// 2 cycles
void Gameboy::loadFromAcc(bool usingC) {
    unsigned short int addr = 0xFF00; 
    unsigned char n = (usingC) ? r.registers[C] : mem[PC];

    if (!usingC) { PC++; }
    addr |= n;

    mem[addr] = r.registers[A];

    printf("%04x\n", addr);
}

// 2 cycles
void Gameboy::loadToAcc(bool usingC) {
    unsigned short int addr = 0xFF00; 
    unsigned char n = (usingC) ? r.registers[C] : mem[PC];

    if (!usingC) { PC++; }
    addr |= n;

    r.registers[A] = mem[addr];

    printf("%04x\n", addr);
}
