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
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);

    mem[addr] = r.registers[value];
}
