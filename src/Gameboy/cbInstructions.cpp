#include "gameboy.h"

// TODO: SRL

RegisterIndex Gameboy::operandToIndex(unsigned char op) {
    if (op > 7) { op -= 8; }

    switch (op) {
        case 0:
            return B;
            break;
        case 1:
            return C;
            break;
        case 2:
            return D;
            break;
        case 3:
            return E;
            break;
        case 4:
            return H;
            break;
        case 5:
            return L;
            break;
        case 6:
            return F; // Acting as HL
            break;
        case 7:
            return A;
            break;
        default:
            return F;
            break;
    }
}

void Gameboy::loadCBInstruction() {
    // Load next byte
    unsigned char byte = mem.read(PC);
    PC++;

    unsigned char firstHalfByte = (byte & 0xF0) >> 4;
    unsigned char secondHalfByte = byte & 0x0F;
    //unsigned char twoBits = (byte & 0b11000000) >> 6;
    //unsigned char fiveBits = (byte & 0b11111000) >> 3;
    unsigned char op = byte & 0b00000111;
    //unsigned char bitIndex = (byte & 0b00111000) >> 3;
    unsigned short bitOffset;

    RegisterIndex r = operandToIndex(op);
    RegisterIndex target = byteToIndex(secondHalfByte);

    switch (firstHalfByte) {
        case 0x00:
            if (secondHalfByte >= 8) {
                RRC(r);
            } else {
                RLC(r);
            }
            break;
        case 0x01:
            if (secondHalfByte >= 8) {
                rotateRegisterRight(target);
            } else {
                rotateRegisterLeft(target);
            }
            break;
        case 0x02:
            if (secondHalfByte < 8) {
                SLA(target);
            } else { 
                SRA(target);
            }
            break;
        case 0x03:
            if (secondHalfByte < 8) {
                swap(target);
            } else {
                srl(target);
            }
            break;
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            bitOffset = (byte & 0b00111000) >> 3; 
            bit(target, bitOffset);
            break;
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
            bitOffset = (byte & 0b00111000) >> 3; 
            resetBit(target, bitOffset);
            break;
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            bitOffset = (byte & 0b00111000) >> 3; 
            setBit(target, bitOffset);
            break;
        default:
            printf("Unknown CB opcode: %02x\n", byte);
            exit(1);
            break;
    }
}

void Gameboy::RLC(RegisterIndex target) {
    if (target == F) {
        RLCHL();
        return;
    }

    unsigned char b7 = r.registers[target] & 0b10000000;
    b7 = b7 >> 7;

    r.registers[target] = r.registers[target] << 1;
    r.registers[target] |= b7;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;
    r.carry = b7;

    r.modifiedFlags = true;    
}

void Gameboy::RRC(RegisterIndex target) {
    if (target == F) {
        RRCHL();
        return;
    }

    unsigned char b0 = r.registers[target] & 0b00000001;
    b0 = b0 << 7;

    r.registers[target] = r.registers[target] >> 1;
    r.registers[target] |= b0;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;
    r.carry = b0 >> 7;

    r.modifiedFlags = true;    
}

void Gameboy::RLCHL() {
    unsigned char val = mem.read(r.getHL());
    unsigned char b7 = val & 0b10000000;
    b7 = b7 >> 7;

    val = val << 1;
    val |= b7;

    mem.write(r.getHL(), val);

    r.zero = (val == 0);
    r.subtract = false;
    r.halfCarry = false;
    r.carry = b7;

    r.modifiedFlags = true;    
}

void Gameboy::RRCHL() {
    unsigned char val = mem.read(r.getHL());
    unsigned char b0 = val & 0b00000001;
    b0 = b0 << 7;

    val = val >> 1;
    val |= b0;

    mem.write(r.getHL(), val);

    r.zero = (val == 0);
    r.subtract = false;
    r.halfCarry = false;
    r.carry = b0 >> 7;

    r.modifiedFlags = true;    
}

void Gameboy::rotateRegisterLeft(RegisterIndex target) {
    if (target == F) {
        rotateRegisterLeftHL();
        return;
    }

    unsigned char carryBit = (r.carry) ? 0b00000001 : 0;
    unsigned char newCarryBit = (r.registers[target] & 0b10000000) >> 7;

    r.registers[target] = (r.registers[target] << 1) | carryBit;
    r.carry = newCarryBit != 0;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
    cycles = 2;
}

void Gameboy::rotateRegisterRight(RegisterIndex target) {
    if (target == F) {
        rotateRegisterRightHL();
        return;
    }

    unsigned char carryBit = (r.carry) ? 0b10000000 : 0;
    unsigned char newCarryBit = r.registers[target] & 0b00000001;

    r.registers[target] = (r.registers[target] >> 1) | carryBit;
    r.carry = newCarryBit != 0;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
    cycles = 2;
}

void Gameboy::rotateRegisterLeftHL() {
    unsigned char val = mem.read(r.getHL());
    unsigned char carryBit = (r.carry) ? 0b00000001 : 0;
    unsigned char newCarryBit = (val & 0b10000000) >> 7;

    val = (val << 1) | carryBit;
    r.carry = newCarryBit != 0;

    mem.write(r.getHL(), val);

    r.zero = (val == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
    cycles = 4;
}

void Gameboy::rotateRegisterRightHL() {
    unsigned char val = mem.read(r.getHL());
    unsigned char carryBit = (r.carry) ? 0b10000000 : 0;
    unsigned char newCarryBit = val & 0b00000001;

    val = (val >> 1) | carryBit;
    r.carry = newCarryBit != 0;

    mem.write(r.getHL(), val);

    r.zero = (val == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
    cycles = 4;
}

void Gameboy::SLA(RegisterIndex target) {
    unsigned char mask = 0b10000000;
    unsigned char* value;
    
    if (target == F) {
        value = &mem.read(r.getHL());
        cycles = 4;
    } else {
        value = &r.registers[target];
        cycles = 2;
    }

    r.carry = ((*value & mask) != 0);

    *value = *value << 1;
    
    r.zero = *value == 0;
    r.subtract = 0;
    r.halfCarry = 0;

    r.modifiedFlags = true;
}

void Gameboy::SRA(RegisterIndex target) {
    unsigned char lowMask = 0b00000001;
    unsigned char highMask = 0b10000000;
    unsigned char* value;
    
    if (target == F) {
        value = &mem.read(r.getHL());
        cycles = 4;
    } else {
        value = &r.registers[target];
        cycles = 2;
    }

    r.carry = ((*value & lowMask) != 0);

    unsigned char highBit = *value & highMask;
    *value = *value >> 1;
    *value |= highBit;
    
    r.zero = *value == 0;
    r.subtract = 0;
    r.halfCarry = 0;

    r.modifiedFlags = true;
}

void Gameboy::bit(RegisterIndex target, unsigned short int bitOffset) {
    unsigned char mask = 0b00000001 << bitOffset;
    unsigned char val;    

    if (target == F) {
        val = mem.read(r.getHL()) & mask;
        cycles = 3;
    } else {
        val = r.registers[target] & mask;
        cycles = 2;
    }

    r.zero = val == 0;
    r.subtract = false;
    r.halfCarry = true;

    r.modifiedFlags = true;
}

void Gameboy::swap(RegisterIndex target) {
    unsigned char* data;

    cycles = 2;
    
    if (target == RegisterIndex::F) {
        data = &mem.read(r.getHL());
        cycles = 4;
    } else {
        data = &r.registers[target];
    }

    unsigned char lowBits = (*data & 0x0F) << 4;
    *data = *data >> 4;
    *data |= lowBits;

    r.zero = *data == 0;
    r.subtract = false;
    r.halfCarry = false;
    r.carry = false;

    r.modifiedFlags = true;
}

void Gameboy::srl(RegisterIndex target) {
    unsigned char* data;
    
    if (target == RegisterIndex::F) {
        data = &mem.read(r.getHL());
        cycles = 4;
    } else {
        data = &r.registers[target];
        cycles = 2;
    }
    
    r.carry = (*data & 0x01) == 1;
    *data = *data >> 1;

    r.zero = *data == 0;
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
}

void Gameboy::resetBit(RegisterIndex target, unsigned short bitOffset) {
    unsigned char mask = 0b00000001 << bitOffset;
    mask = ~mask;

    unsigned char* data;
    if (target == RegisterIndex::F) {
        data = &mem.read(r.getHL());
        cycles = 4;
    } else {
        data = &r.registers[target];
        cycles = 2;
    }

    *data &= mask;
}

void Gameboy::setBit(RegisterIndex target, unsigned short bitOffset) {
    unsigned char mask = 0b00000001 << bitOffset;

    unsigned char* data;
    if (target == RegisterIndex::F) {
        data = &mem.read(r.getHL());
        cycles = 4;
    } else {
        data = &r.registers[target];
        cycles = 2;
    }

    *data |= mask;
}
