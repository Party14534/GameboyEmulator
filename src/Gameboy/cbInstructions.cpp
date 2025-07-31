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

    RegisterIndex r = operandToIndex(op);

    switch (firstHalfByte) {
        case 0x00:
            if (secondHalfByte >= 8) {
                RRC(r);
            } else {
                RLC(r);
            }
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
    unsigned char b1 = r.registers[target] & 0b00000001;
    b1 = b1 << 7;

    r.registers[target] = r.registers[target] >> 1;
    r.registers[target] |= b1;

    r.zero = (r.registers[target] == 0);
    r.subtract = false;
    r.halfCarry = false;
    r.carry = b1 >> 7;

    r.modifiedFlags = true;    
}

