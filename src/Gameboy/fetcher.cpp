#include "gameboy.h"

Fetcher::Fetcher(GameboyMem& _mem) : mem(_mem) { }

void Fetcher::setup() {
    FIFO = std::vector<Pixel>(16);
    tileData = std::vector<unsigned char>(8);
    objectData = std::vector<unsigned char>(8);
    cycles = 0;
    state = ReadTileID;
    videoBuffer = std::vector<sf::Color>(160 * 144);

    BGP = &mem.read(0xff47);
    OBP0 = &mem.read(0xff48);
    OBP1 = &mem.read(0xff49);
}

void Fetcher::Start(unsigned short int _mapAddr, unsigned short int _dataAddr,
        unsigned char _tileOffset,
        unsigned short int _tileLine, bool _signedId) {
    tileOffset = _tileOffset;
    mapAddr = _mapAddr;
    dataAddr = _dataAddr;
    tileLine = _tileLine;
    signedId = _signedId;
    state = ReadTileID;
    cycles = 0;
    
    // Clear FIFO between calls
    FIFO.clear();
}

void Fetcher::FetchObject(OAMObject _object, unsigned char offset, unsigned char line) {
    object = _object;
    objectOffset = offset;
    objectLine = line;
    oldState = state;
    state = ReadObjectID;
}

// Runs at half the clock speed of the PPU
void Fetcher::Tick() {
    cycles++;
    if (cycles < 2) {
        return;
    }
    cycles = 0;
    unsigned char flags;
    unsigned char adjustedLine;

    switch (state) {
        case ReadTileID:
            // Read tile's number from background
            tileID = mem.read(mapAddr + tileOffset);
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

        case ReadObjectID:
            objectId = mem.read(object.addr + 2);
            state = ReadObjectFlags;
            break;

        case ReadObjectFlags:
            flags = mem.read(object.addr + 3);
            object.attributes.priority = (flags & (1 << 7)) != 0;
            object.attributes.flipY = (flags & (1 << 6)) != 0;
            object.attributes.flipX = (flags & (1 << 5)) != 0;
            object.attributes.palette = (flags & (1 << 4)) != 0;
            object.attributes.bank = (flags & (1 << 3)) != 0;

            adjustedLine = objectLine;
            if ((mem.read(LCDC_ADDR) & 4) != 0) {
                if (object.attributes.flipY) {
                    adjustedLine = 15 - objectLine;
                }

                // Now select tile based on adjusted line
                if (adjustedLine < 8) {
                    objectId &= 0xfe;  // Top tile
                } else {
                    objectId |= 1;     // Bottom tile
                    adjustedLine -= 8;
                }

                objectLine = adjustedLine;
            } else if (object.attributes.flipY) {
                objectLine = 7 - objectLine;
            }

            state = ReadObjectData0;
            break;

        case ReadObjectData0:
            readObjectData(0);            
            state = ReadObjectData1;
            break;

        case ReadObjectData1:
            readObjectData(1);
            state = MixFIFO;
            break;

        case MixFIFO:
            MixInFifo();
            state = oldState;
            break;
    }
}

void Fetcher::readTileData(unsigned short int addrOffset) {
    // A tile's graphical data takes 16 bytes so we compute an offset
    // to find data for desired tile is
    unsigned short int offset;
    if (signedId) {
        offset = dataAddr + (static_cast<signed char>(tileID) * 16);
    } else {
        offset = dataAddr + (tileID * 16);
    }

    // Compute final address to read by finding out which of the
    // 8-pixel rows of the tile we want.
    unsigned short int addr = offset + (tileLine * 2);

    // Finally read the first byte of graphical data
    unsigned char data = mem.read(addr + addrOffset);
    for (unsigned short int bitPos = 0; bitPos < 8; bitPos++) {
        if (!addrOffset) {
            tileData[bitPos] = (data >> bitPos) & 1;
        } else {
            tileData[bitPos] |= ((data >> bitPos) & 1) << 1;
        }
    }
}

void Fetcher::readObjectData(unsigned short int addrOffset) {
    /*unsigned char objHeight = 8;
    if ((mem.read(LCDC_ADDR) & 4) != 0) { objHeight = 16; }*/

    // A tile's graphical data takes 16 bytes so we compute an offset
    // to find data for desired tile is
    unsigned short int offset = 0x8000 + (objectId * 16);

    // Compute final address to read by finding out which of the
    // 8-pixel rows of the tile we want.
    unsigned short int addr = offset + (objectLine * 2);

    // Finally read the first byte of graphical data
    unsigned char data = mem.read(addr + addrOffset);
    for (unsigned short int bitPos = 0; bitPos < 8; bitPos++) {
        unsigned short int dataIndex;
        if (!object.attributes.flipX) {
            dataIndex = 7 - bitPos;
        } else {
            dataIndex = bitPos;
        }

        if (!addrOffset) {
            objectData[dataIndex] = (data >> bitPos) & 1;
        } else {
            objectData[dataIndex] |= ((data >> bitPos) & 1) << 1;
        }
    }
}

void Fetcher::MixInFifo() {
    if (FIFO.size() < 8) { return; } 

    bool bgCover = object.attributes.priority != 0; // TODO: Comeback to this

    for (unsigned char i = objectOffset; i < 8; i++) {
        int testIndex = 7 - (i - objectOffset);
        Pixel pixel = {objectData[testIndex], object.attributes.palette, bgCover, true};
        // MIXING
        unsigned char offset = i - objectOffset;
        unsigned char size = FIFO.size();
        if (size > 8) { size = 8; }

        unsigned char index = size - offset - 1;
        Pixel current = FIFO[index];

        // Skip transparent object pixels
        if (pixel.color == 0 || (mem.read(LCDC_ADDR) & 2) == 0) {
            continue;
        }

        // Don't overwrite existing object pixels (first object wins)
        if (current.isObject) {
            continue;
        }

        // OBJ-to-BG Priority: If set and BG color != 0, don't draw object
        if (bgCover && current.color != 0) {
            continue;
        }

        // Draw the object pixel
        FIFO[index] = pixel;

        /*
        if (current.palette == 0) { // 0 is the Background transparency color
            if (current.color != 0 && bgCover) {
                continue;
            }

            FIFO[index] = pixel;
        }*/
    }

}

void Fetcher::pushToFIFO() {
    if (FIFO.size() > 8) { return; }

    for (int i = 7; i >= 0; i--) {
        Pixel p;
        p.color = tileData[i];
        p.palette = 0;
        p.priority = false;
        p.isObject = false;

        if ((mem.read(LCDC_ADDR) & 1) == 0) { // BG/WINDOW enable
            p.color = 0;
        }

        FIFO.push_back(p);
    }

    tileOffset = (tileOffset + 1) % 32;
}

void Fetcher::popFIFO() {
    if (FIFO.size() < 8) {
        printf("FIFO Underrun\n");
        exit(1);
    }

    FIFO.erase(FIFO.begin());
    //FIFO.pop_back();
}


void Fetcher::pushToVBuffer() {
    // Push to video buffer 
    Pixel pixel = FIFO.front();
    FIFO.erase(FIFO.begin());

    // BGP contains four 2bit values
    // TODO: Choose palette

    unsigned char paletteIndex;
    if (!pixel.isObject) {
        paletteIndex = (*BGP >> (pixel.color * 2)) & 3;
    } else if (pixel.palette == 0) {
        paletteIndex = (*OBP0 >> (pixel.color * 2)) & 3;
    } else {
        paletteIndex = (*OBP1 >> (pixel.color * 2)) & 3;
    }

    sf::Color c = paletteOne[paletteIndex];

    videoBuffer[vBufferIndex] = c;
    vBufferIndex = (vBufferIndex + 1) % (160 * 144);
}
