#include "gameboy.h"

// 0x80 - 0x85, 0x87 - 0x8D, 0x8F
void Gameboy::add(RegisterIndex target, bool carry) {
    if (target == RegisterIndex::F) {
        printf("Error: trying to add register F\n");
        exit(1);
    }

    unsigned char value = r.registers[target];
    if (carry) {
        value += r.carry;
    }

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] += value;
    
    // Set flags
    if (!r.registers[RegisterIndex::A]) { r.zero = 0x01; }
    r.subtract = false;
    r.carry = (oldVal > r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) + (value & 0x0F)) > 0x0F);
    printf("%02x %02x\n", oldVal & 0x0F, value & 0x0F);
}

// 0x86 && 0x8E
void Gameboy::addFromMemory(bool carry) {
    unsigned short int addr = r.registers[RegisterIndex::L];
    
    addr += (r.registers[RegisterIndex::H] << 8);
    unsigned char value = mem[addr];
    if (carry) {
        value += r.carry;
    }

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] += value;
    
    // Set flags
    if (!r.registers[RegisterIndex::A]) { r.zero = 0x01; }
    r.subtract = false;
    r.carry = (oldVal > r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) + 
        (value & 0x0F)) > 0x0F);
}

// 0x90 - 0x95, 0x97 - 0x9D, 0x9F
void Gameboy::subtract(RegisterIndex target, bool carry) {
    if (target == RegisterIndex::F) {
        printf("Error: trying to add register F\n");
        exit(1);
    }

    unsigned char value = r.registers[target];
    if (carry) {
        value += r.carry;
    }

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] -= value;
    
    // Set flags
    if (!r.registers[RegisterIndex::A]) { r.zero = 0x01; }
    r.subtract = false;
    r.carry = (oldVal < r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);
}

// 0x86 && 0x8E
void Gameboy::subtractFromMemory(bool carry) {
    unsigned short int addr = r.registers[RegisterIndex::L];
    
    addr += (r.registers[RegisterIndex::H] << 8);
    unsigned char value = mem[addr];
    if (carry) {
        value += r.carry;
    }

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] -= value;
    
    // Set flags
    if (!r.registers[RegisterIndex::A]) { r.zero = 0x01; }
    r.subtract = false;
    r.carry = (oldVal < r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);
}
