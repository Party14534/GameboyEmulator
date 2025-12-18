#include "gameboy.h"

GameboyMem::GameboyMem(unsigned short int& _PC) {
    mem = std::vector<unsigned char>(0xFFFF + 1);
    bootRomMem = std::vector<unsigned char>(0xFF);
    bootFinished = &mem[0xFF50];
    PC = &_PC;
}

unsigned char& GameboyMem::read(unsigned short int addr) {
    if (!*bootFinished && addr <= 0xFF) {
        return bootRomMem[addr];
    }

    if (DOCTOR_LOGGING && addr == LY_ADDR) {
        mem[addr] = 0x90;
    }

    return mem[addr];
}

void GameboyMem::write(unsigned short int addr, unsigned char val) {
    mem[addr] = val;

    switch(addr) {
        case 0xFF01:
        case 0xFF02:
            if (LOG_SERIAL) {
                printf("SERIAL: %04x %d\n", val, val);
            }
            break;
        case LCDC_ADDR:
            printf("%x\n", val);
            break;
        case 0xFF50:
            if (val == 0) { return; }
            break;

            *PC = 0x100;
            return;
        default:
            break;
    }
    if (addr == 0xFF50 && val != 0) {
        *PC = 0x100;
    }
}
