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

    if (LOGGING) printf("ADD %d FROM REGISTER %c TO REGISTER A\n", value, target + 65);
    
    // Set flags
    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = false;
    r.carry = (oldVal > r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) + (value & 0x0F)) > 0x0F);

    r.modifiedFlags = true;
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

    if (LOGGING) printf("ADD %d FROM ADDR: 0x%04x TO REGISTER A\n", value, addr);
    
    // Set flags
    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = false;
    r.carry = (oldVal > r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) + 
        (value & 0x0F)) > 0x0F);

    r.modifiedFlags = true;
}

void Gameboy::addImmediate() {
    unsigned char value = mem[PC];
    PC++;

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] += value;

    if (LOGGING) printf("ADD IMMEDIATE %d TO REGISTER A\n", value);
    
    // Set flags
    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = false;
    r.carry = (oldVal > r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) + (value & 0x0F)) > 0x0F);

    r.modifiedFlags = true;
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

    if (LOGGING) printf("SUB %d GOTTEN FROM REGISTER %c TO REGISTER A\n", value, target + 65);
    
    // Set flags
    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = true;
    r.carry = (oldVal < r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);

    r.modifiedFlags = true;
}

// 0x96 && 0x9E
void Gameboy::subtractFromMemory(bool carry) {
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);

    unsigned char value = mem[addr];
    if (carry) {
        value += r.carry;
    }

    unsigned char oldVal = r.registers[RegisterIndex::A];
    r.registers[RegisterIndex::A] -= value;
    if (LOGGING) printf("SUB %d GOTTEN FROM ADDR: 0x%04x TO REGISTER A\n", value, addr);
    
    // Set flags
    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = true;
    r.carry = (oldVal < r.registers[RegisterIndex::A]); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);

    r.modifiedFlags = true;
}

// Misc arithmetic functions

void Gameboy::incRegister(RegisterIndex target, char val) {
    unsigned char oldVal = r.registers[target];

    unsigned char result = (unsigned char)(oldVal + val);

    r.registers[target] = result;

    if (val > 0) {
        if (LOGGING) printf("INC %d TO REGISTER %c NOW: 0x%02x\n", val, target + 65, r.registers[target]);
    } else {
        if (LOGGING) printf("DEC %d TO REGISTER %c NOW: 0x%02x\n", val, target + 65, r.registers[target]);
    }
    
    // Set flags
    r.zero = (result == 0);
    r.subtract = (val < 0);
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    if (val < 0) {
        // val is negative so we set it to positive for the half carry check
        val *= -1;
        r.halfCarry = (((oldVal & 0x0F) - (val & 0x0F)) & 0x10);
    } else {
        r.halfCarry = (((oldVal & 0x0F) + (val & 0x0F)) > 0x0F);
    }

    r.modifiedFlags = true;
}

void Gameboy::incMemory(char val) {
    unsigned short int addr = r.getHL();
    unsigned char oldVal = mem[addr];

    unsigned char result = (unsigned char)(oldVal + val);

    if (LOGGING) printf("INC %d TO ADDR: 0x%04x\n", val, addr);
    
    // Set flags
    r.zero = (result == 0);
    r.subtract = (val < 0);
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    if (val < 0) {
        r.halfCarry = (((oldVal & 0x0F) - (val & 0x0F)) & 0x10);
    } else {
        r.halfCarry = (((oldVal & 0x0F) + (val & 0x0F)) > 0x0F);
    }

    mem[addr] = result;

    r.modifiedFlags = true;
}

/*
 * Logical Functions
 */

// 0xA0 - 0xA5, 0xA7
void Gameboy::bitwiseAnd(RegisterIndex target) {
    r.registers[RegisterIndex::A] &= r.registers[target];

    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = 0;
    r.halfCarry = 1;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 2 cycles
void Gameboy::bitwiseAndImmediate() {
    unsigned char n = mem[PC];
    PC++;

    r.registers[RegisterIndex::A] &= n;

    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = 0;
    r.halfCarry = 1;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xA6
void Gameboy::bitwiseAndFromMemory() {
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);
    
    unsigned char value = mem[addr];

    r.registers[RegisterIndex::A] &= value;

    r.zero = (!r.registers[RegisterIndex::A]);
    r.subtract = 0;
    r.halfCarry = 1;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xA8 - 0xAD, 0xAF
void Gameboy::bitwiseXor(RegisterIndex target) {
    r.registers[RegisterIndex::A] ^= r.registers[target];

    if (LOGGING) printf("XOR REGISTER A WITH REGISTER %c\n", target + 65);

    r.zero = (!r.registers[RegisterIndex::A]);
    r.subtract = 0;
    r.halfCarry = 0;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xAE
void Gameboy::bitwiseXorFromMemory() {
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);
    
    unsigned char value = mem[addr];

    if (LOGGING) printf("XOR REGISTER A WITH ADDR 0x%04x\n", addr);

    r.registers[RegisterIndex::A] ^= value;

    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = 0;
    r.halfCarry = 0;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xEE
void Gameboy::bitwiseXorImmediate() {
    unsigned char value = mem[PC];
    PC++;

    if (LOGGING) printf("XOR REGISTER A WITH VALUE 0x%02x\n", value);

    r.registers[RegisterIndex::A] ^= value;

    r.zero = (r.registers[RegisterIndex::A] == 0);
    r.subtract = 0;
    r.halfCarry = 0;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xB0 - 0xB5, 0xB7
void Gameboy::bitwiseOr(RegisterIndex target) {
    r.registers[RegisterIndex::A] |= r.registers[target];

    if (LOGGING) printf("OR REGISTER A WITH REGISTER %c\n", target + 65);

    r.zero = (!r.registers[RegisterIndex::A]);
    r.subtract = 0;
    r.halfCarry = 0;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xB6
void Gameboy::bitwiseOrFromMemory() {
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);
    
    unsigned char value = mem[addr];

    if (LOGGING) printf("OR REGISTER A WITH ADDR 0x%04x\n", addr);

    r.registers[RegisterIndex::A] |= value;

    r.zero = (!r.registers[RegisterIndex::A]);
    r.subtract = 0;
    r.halfCarry = 0;
    r.carry = 0;

    r.modifiedFlags = true;
}

// 0xB8 - 0xBD, 0xBF
void Gameboy::compare(RegisterIndex target) {
    unsigned char value = r.registers[target];

    unsigned char oldVal = r.registers[RegisterIndex::A];
    unsigned char result = oldVal - value;

    if (LOGGING) printf("COMPARE REGISTER A WITH REGISTER %c\n", target + 65);
    
    // Set flags
    r.zero = (result == 0);
    r.subtract = true;
    r.carry = (oldVal < result); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);

    r.modifiedFlags = true;
}

// 0xBE
void Gameboy::compareFromMemory() {
    unsigned short int addr = r.registers[RegisterIndex::L];
    addr += (r.registers[RegisterIndex::H] << 8);

    unsigned char value = mem[addr];

    unsigned char oldVal = r.registers[RegisterIndex::A];
    unsigned char result = oldVal - value;

    if (LOGGING) printf("COMPARE REGISTER A WITH ADDR 0x%04x\n", addr);
    
    // Set flags
    r.zero = (result == 0);
    r.subtract = true;
    r.carry = (oldVal < result); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);

    r.modifiedFlags = true;
}

void Gameboy::compareN() {
    unsigned char value = mem[PC];
    PC++;

    unsigned char oldVal = r.registers[RegisterIndex::A];
    unsigned char result = oldVal - value;

    if (LOGGING) printf("COMPARE REGISTER A WITH ADDR 0x%04x : RESULT: 0x%02x\n", PC-1, result);
    
    // Set flags
    r.zero = (result == 0);
    r.subtract = true;
    r.carry = (oldVal < result); // Overflow
    // Half Carry is set if adding the lower nibbles of the value and register
    // A together result in a value bigger than 0xF. If the result is larger 
    // than 0xF then the addition caused a carry from the lower nibble to the
    // upper nibble.
    r.halfCarry = (((oldVal & 0x0F) - (value & 0x0F)) & 0x10);

    r.modifiedFlags = true;
}

