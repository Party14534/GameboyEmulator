#include "gameboy.h"

GameboyMem::GameboyMem() {
    mem = std::vector<unsigned char>(0xFFFF);
}

unsigned char& GameboyMem::operator[](int i) {
    return mem[i];
}
