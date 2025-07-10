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
            switch (secondHalfByte) {
                case 0x00:
                    Add(RegisterIndex::B, false);
                    break;
                case 0x01:
                    Add(RegisterIndex::C, false);
                    break;
                case 0x02:
                    Add(RegisterIndex::D, false);
                    break;
                case 0x03:
                    Add(RegisterIndex::E, false);
                    break;
                case 0x04:
                    Add(RegisterIndex::H, false);
                    break;
                case 0x05:
                    Add(RegisterIndex::L, false);
                    break;
                case 0x06:
                    AddFromMemory(false);
                    break;
                case 0x07:
                    Add(RegisterIndex::A, false);
                    break;
                case 0x08:
                    Add(RegisterIndex::B, true);
                    break;
                case 0x09:
                    Add(RegisterIndex::C, true);
                    break;
                case 0x0A:
                    Add(RegisterIndex::D, true);
                    break;
                case 0x0B:
                    Add(RegisterIndex::E, true);
                    break;
                case 0x0C:
                    Add(RegisterIndex::H, true);
                    break;
                case 0x0D:
                    Add(RegisterIndex::L, true);
                    break;
                case 0x0E:
                    AddFromMemory(true);
                    break;
                case 0x0F:
                    Add(RegisterIndex::A, true);
                    break;
                default:
                    printf("Unknown instruction %04x\n", secondHalfByte);
                    exit(1);
            }
            break;
        default:
            printf("Unknown instruction %04x\n", instruction);
            exit(1);
    }
}
