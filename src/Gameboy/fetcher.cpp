#include "gameboy.h"

Fetcher::Fetcher(unsigned char* _mem) {
    mem = _mem;
    FIFO = std::vector<Pixel>(8);
    cycles = 0;
    state = ReadTileID;
}

void Fetcher::Start(unsigned short int _mapAddr, unsigned short int _tileLine) {
    tileIndex = 0;
    mapAddr = _mapAddr;
    tileLine = _tileLine;
    state = ReadTileID;

    FIFO.clear();
}

// Runs at half the clock speed of the PPU
void Fetcher::Tick() {
    cycles++;
    if (cycles < 2) {
        return;
    }
    cycles = 0;

    switch (state) {
        case ReadTileID:
            // Read tile's number from background
            tileID = mem[mapAddr + tileIndex];
            state = ReadTileData0;
            break;
        case ReadTileData0:
            state = ReadTileData1;
            break;
        case ReadTileData1:
            state = PushToFIFO;
            break;
        case PushToFIFO:
            state = ReadTileID;
            break;
    }
}
