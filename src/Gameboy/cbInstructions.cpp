#include "gameboy.h"

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
    unsigned char byte = mem[PC];
    PC++;

    unsigned char firstHalfByte = (byte & 0xF0) >> 4;
    unsigned char secondHalfByte = byte & 0x0F;
    unsigned char twoBits = (byte & 0b11000000) >> 6;
    unsigned char fiveBits = (byte & 0b11111000) >> 3;
    unsigned char op = byte & 0b00000111;
    unsigned char bitIndex = (byte & 0b00111000) >> 3;
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
            bitOffset = (firstHalfByte - 0x04) * 2;
            bitOffset = (secondHalfByte < 0x07) ? bitOffset : bitOffset + 1;
            
            bit(target, bitOffset);
            break;
        default:
            printf("Unknown CB opcode: %02x\n", byte);
            exit(1);
            break;
    }
}

void Gameboy::RLC(RegisterIndex target) {
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

void Gameboy::rotateRegisterLeft(RegisterIndex target) {
    unsigned char carryBit = (r.carry) ? 0b00000001 : 0;
    unsigned char newCarryBit = (r.registers[target] & 0b10000000) >> 7;

    r.registers[target] = (r.registers[target] << 1) | carryBit;
    r.carry = newCarryBit != 0;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
}

void Gameboy::rotateRegisterRight(RegisterIndex target) {
    unsigned char carryBit = (r.carry) ? 0b10000000 : 0;
    unsigned char newCarryBit = r.registers[target] & 0b00000001;

    r.registers[target] = (r.registers[target] >> 1) | carryBit;
    r.carry = newCarryBit != 0;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
}

void Gameboy::bit(RegisterIndex target, unsigned short int bitOffset) {
    unsigned char mask = 0b00000001 << bitOffset;
    unsigned char val;    

    //printf("%04x %d\n", mask, bitOffset);
    if (target == F) {
        val = mem[r.getHL()] & mask;
    } else {
        val = r.registers[target] & mask;
    }

    r.zero = val == 0;
    r.subtract = false;
    r.halfCarry = true;

    r.modifiedFlags = true;
}

void Gameboy::swap(RegisterIndex target) {
    unsigned char* data;
    
    if (target == RegisterIndex::F) {
        printf("0x%04x\n", r.getHL());
        data = &mem[r.getHL()];
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
        data = &mem[r.getHL()];
    } else {
        data = &r.registers[target];
    }
    
    r.carry = (*data & 0x01) == 1;
    *data = *data >> 1;

    r.zero = *data == 0;
    r.subtract = false;
    r.halfCarry = false;

    r.modifiedFlags = true;
}
