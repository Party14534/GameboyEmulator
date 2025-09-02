#include "gameboy.h"

PPU::PPU(std::vector<unsigned char>& gameboyMem) {
    background = std::vector<unsigned char>(256 * 256);
    window = std::vector<unsigned char>(256 * 256);
    viewport = std::vector<unsigned char>(160 * 144);

    graphicsData = &gameboyMem[0x8000];
    backgroundMap1 = &gameboyMem[0x9800];
    backgroundMap2 = &gameboyMem[0x9C00];
    OAMemory = &gameboyMem[0xFE00];

    WX = &gameboyMem[0xFF4B];
    WY = &gameboyMem[0xFF4A];
}

// H-Blank
// After drawing mode (mode 2)
void PPU::mode0() {
    // TODO: pad timing to 456 T-Cycles
}

// V-Blank
void PPU::mode1() {
    // TODO: Takes place at end of every frame for 10 * 456 T-cycles
}

// OAM Scan
void PPU::mode2() {

}

// Drawing mode
void PPU::mode3() {

}

