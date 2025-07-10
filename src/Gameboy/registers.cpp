#include "gameboy.h"

// 16 bit functions
void Registers::setAF(short unsigned int val) {
    registers[RegisterIndex::A] = (0xFF00 & val) >> 8;
    registers[RegisterIndex::F] = 0x00FF & val;
}

void Registers::setBC(short unsigned int val) {
    registers[RegisterIndex::B] = (0xFF00 & val) >> 8;
    registers[RegisterIndex::C] = 0x00FF & val;
}

void Registers::setDE(short unsigned int val) {
    registers[RegisterIndex::D] = (0xFF00 & val) >> 8;
    registers[RegisterIndex::E] = 0x00FF & val;
}

void Registers::setHL(short unsigned int val) {
    registers[RegisterIndex::H] = (0xFF00 & val) >> 8;
    registers[RegisterIndex::L] = 0x00FF & val;
}

// Flag register functions
unsigned char Registers::fRtoU8() {
    unsigned char byte;

    byte |= zero << 7;
    byte |= subtract << 6;
    byte |= halfCarry << 5;
    byte |= carry << 4;

    return byte;
}

void Registers::u8toFR(unsigned char byte) {
    zero = (byte >> 7) & 0b00000001; 
    subtract = (byte >> 6) & 0b00000001; 
    halfCarry = (byte >> 5) & 0b00000001; 
    carry = (byte >> 4) & 0b00000001; 
}

