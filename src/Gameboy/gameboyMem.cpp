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

    return mem[addr];
}

void GameboyMem::write(unsigned short int addr, unsigned char val) {
    mem[addr] = val;

    if (addr == 0xFF50) {
        printf("lol\n");
    }
    if (addr == 0xFF50 && val != 0) {
        *PC = 0x100;
    }
}
