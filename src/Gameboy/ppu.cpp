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

void PPU::main() {
    switch (state) {
        case OAMSearch:
            mode2();
            break;
        case PixelTransfer:
            mode3();
            break;
        case HBlank:
            mode0();
            break;
        case VBlank:
            mode1();
            break;
    }  
}

// H-Blank
// After drawing mode (mode 2)
void PPU::mode0() {
    // TODO: pad timing to 456 T-Cycles
    // Enter VBlank if at end of screen
    if (cycles == 456) {
        cycles = 0;
        LY++;
        if (LY == 144) { state = VBlank; }
        else { state = OAMSearch; }
    }
}

// V-Blank
void PPU::mode1() {
    // TODO: Takes place at end of every frame for 10 * 456 T-cycles
    if (cycles == 456) {
        cycles = 0;
        LY++;
        if (LY == 153) {
            LY = 0;
            state = OAMSearch;
        }
    }
}

// OAM Scan
void PPU::mode2() {
    // TODO: wait for 80 T-Cycles
    if (cycles == 80) {
        state = PixelTransfer;
    }
}

// Drawing mode
void PPU::mode3() {
    if (cycles == 80) {
        x = 0;
        unsigned short int tileLine = LY % 8;
        unsigned short int tileMapRowAddr = 0x9800 + ((LY / 8) * 32);
    }

    x++;
    if (x == 160) {
        state = HBlank;
    }
}

