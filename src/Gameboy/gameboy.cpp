#include "gameboy.h"
#include <cstdio>

Gameboy::Gameboy(std::string _romPath) : romPath(_romPath) {
    r.registers = std::vector<unsigned char>(8);
    mem = std::vector<unsigned char>(0xFFFF); // 65535
}

void Gameboy::FDE() {
    unsigned char instruction = fetch();

    decode(instruction);
}

unsigned char Gameboy::fetch() {
    unsigned char instruction = mem[PC];

    PC++;

    return instruction;
}

void Gameboy::decode(unsigned char instruction) {
    unsigned char firstHalfByte = instruction & 0xF0;
    unsigned char secondHalfByte = instruction & 0x0F;

    switch (firstHalfByte) {
        case 0x80:
            call8XInstructions(secondHalfByte);
            break;
        case 0x90:
            call9XInstructions(secondHalfByte);
            break;
        default:
            printf("Unknown instruction %04x\n", instruction);
            exit(1);
    }
}

/*
 * Instruction calling
 */
void Gameboy::call8XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            add(RegisterIndex::B, false);
            break;
        case 0x01:
            add(RegisterIndex::C, false);
            break;
        case 0x02:
            add(RegisterIndex::D, false);
            break;
        case 0x03:
            add(RegisterIndex::E, false);
            break;
        case 0x04:
            add(RegisterIndex::H, false);
            break;
        case 0x05:
            add(RegisterIndex::L, false);
            break;
        case 0x06:
            addFromMemory(false);
            break;
        case 0x07:
            add(RegisterIndex::A, false);
            break;
        case 0x08:
            add(RegisterIndex::B, true);
            break;
        case 0x09:
            add(RegisterIndex::C, true);
            break;
        case 0x0A:
            add(RegisterIndex::D, true);
            break;
        case 0x0B:
            add(RegisterIndex::E, true);
            break;
        case 0x0C:
            add(RegisterIndex::H, true);
            break;
        case 0x0D:
            add(RegisterIndex::L, true);
            break;
        case 0x0E:
            addFromMemory(true);
            break;
        case 0x0F:
            add(RegisterIndex::A, true);
            break;
        default:
            printf("Unknown instruction %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call9XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            subtract(RegisterIndex::B, false);
            break;
        case 0x01:
            subtract(RegisterIndex::C, false);
            break;
        case 0x02:
            subtract(RegisterIndex::D, false);
            break;
        case 0x03:
            subtract(RegisterIndex::E, false);
            break;
        case 0x04:
            subtract(RegisterIndex::H, false);
            break;
        case 0x05:
            subtract(RegisterIndex::L, false);
            break;
        case 0x06:
            subtractFromMemory(false);
            break;
        case 0x07:
            subtract(RegisterIndex::A, false);
            break;
        case 0x08:
            subtract(RegisterIndex::B, true);
            break;
        case 0x09:
            subtract(RegisterIndex::C, true);
            break;
        case 0x0A:
            subtract(RegisterIndex::D, true);
            break;
        case 0x0B:
            subtract(RegisterIndex::E, true);
            break;
        case 0x0C:
            subtract(RegisterIndex::H, true);
            break;
        case 0x0D:
            subtract(RegisterIndex::L, true);
            break;
        case 0x0E:
            subtractFromMemory(true);
            break;
        case 0x0F:
            subtract(RegisterIndex::A, true);
            break;
        default:
            printf("Unknown instruction %04x\n", secondHalfByte);
            exit(1);
    }
}
