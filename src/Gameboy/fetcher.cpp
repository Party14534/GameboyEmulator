#include "gameboy.h"

void Fetcher::setup(unsigned char* _mem) {
    mem = _mem;
    FIFO = std::vector<Pixel>(16);
    tileData = std::vector<unsigned char>(8);
    cycles = 0;
    state = ReadTileID;
    videoBuffer = std::vector<sf::Color>(160 * 144);

    BGP = &mem[0xff47];
}

void Fetcher::Start(unsigned short int _mapAddr, unsigned short int _tileLine) {
    tileIndex = 0;
    mapAddr = _mapAddr;
    tileLine = _tileLine;
    state = ReadTileID;

    // Clear FIFO between calls
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
            readTileData(0);
            state = ReadTileData1;
            break;
        case ReadTileData1:
            readTileData(1);
            state = PushToFIFO;
            break;
        case PushToFIFO:
            pushToFIFO();
            state = ReadTileID;
            break;
    }
}

void Fetcher::readTileData(unsigned short int addrOffset) {
    // A tile's graphical data takes 16 bytes so we compute an offset
    // to find data for desired tile is
    unsigned short int offset = 0x8000 + (tileID * 16);

    // Compute final address to read by finding out which of the
    // 8-pixel rows of the tile we want.
    unsigned short int addr = offset + (tileLine * 2);

    // Finally read the first byte of graphical data
    unsigned char data = mem[addr + addrOffset];
    for (unsigned short int bitPos = 0; bitPos <= 7; bitPos++) {
        if (!addrOffset) {
            tileData[bitPos] = (data >> bitPos) & 1;
        } else {
            tileData[bitPos] |= ((data >> bitPos) & 1) << 1;
        }
    }
}

void Fetcher::pushToFIFO() {
    if (FIFO.size() > 8) { return; }
    // We stored pixels least significant to most significant so we push in
    // reverse order

    for (int i = 0; i < 8; i++) {
        Pixel p;
        p.color = tileData[i];
        FIFO.push_back(p);
    }

    // Push to video buffer separately cause it's an experiment
    /*for (int i = 0; i < 8; i++) {
        Pixel pixel = FIFO.back();
        FIFO.pop_back();

        sf::Color c = paletteOne[pixel.color];

        videoBuffer[vBufferIndex] = c;
        vBufferIndex = (vBufferIndex + 1) % (160 * 144);
    }*/

    tileIndex++;
}


void Fetcher::pushToVBuffer() {
    // Push to video buffer 
    Pixel pixel = FIFO.back();
    FIFO.pop_back();

    // BGP contains four 2bit values
    unsigned char paletteIndex = (*BGP >> (pixel.color * 2)) & 3;
    sf::Color c = paletteOne[paletteIndex];

    videoBuffer[vBufferIndex] = c;
    vBufferIndex = (vBufferIndex + 1) % (160 * 144);
}
