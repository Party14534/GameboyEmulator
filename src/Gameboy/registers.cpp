#include "gameboy.h"

void Registers::setFlags() {
    zero = registers[RegisterIndex::F] & 0b10000000;
    subtract = registers[RegisterIndex::F] & 0b01000000;
    halfCarry = registers[RegisterIndex::F] & 0b00100000;
    carry = registers[RegisterIndex::F] & 0b00010000;
}

void Registers::setF() {
    unsigned char f = 0x00;
    f |= zero << 7;
    f |= subtract << 6;
    f |= halfCarry << 5;
    f |= carry << 4;

    registers[RegisterIndex::F] = f;
}

// 16 bit functions
void Registers::setAF(short unsigned int val) {
    registers[RegisterIndex::A] = (0xFF00 & val) >> 8;
    registers[RegisterIndex::F] = 0x00F0 & val; // F register has no lower nibble

    // Set the boolean flags after setting F register
    setFlags();
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

short unsigned int Registers::getAF() {
    short unsigned int val;
    
    val = registers[RegisterIndex::A] << 8;
    
    // Make sure F is updated with values from flags
    setF();
    val += registers[RegisterIndex::F];

    return val;
}

short unsigned int Registers::getBC() {
    short unsigned int val;
    
    val = registers[RegisterIndex::B] << 8;
    val += registers[RegisterIndex::C];

    return val;
}

short unsigned int Registers::getDE() {
    short unsigned int val;
    
    val = registers[RegisterIndex::D] << 8;
    val += registers[RegisterIndex::E];

    return val;
}

short unsigned int Registers::getHL() {
    short unsigned int val;
    
    val = registers[RegisterIndex::H] << 8;
    val += registers[RegisterIndex::L];

    return val;
}

// Flag register functions
unsigned char Registers::fRtoU8() {
    unsigned char byte = 0;

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

