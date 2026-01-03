#include "gameboy.h"
unsigned short int FetchingSprite = ReadObjectID | ReadObjectFlags | ReadObjectData0 | ReadObjectData1 | MixFIFO;
// Bit 0 - BG/Window Display/Priority     (0=Off, 1=On)
unsigned char LCDCBGDisplay = 1;
// Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
unsigned char LCDCSpriteDisplayEnable = 1 << 1;
// Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
unsigned char LCDCSpriteSize = 1 << 2;
// Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
unsigned char LCDCBGTileMapDisplaySelect = 1 << 3;
// Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
unsigned char LCDCBGWindowTileDataSelect = 1 << 4;
// Bit 5 - Window Display Enable          (0=Off, 1=On)
unsigned char LCDCWindowDisplayEnable = 1 << 5;
// Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
unsigned char LCDCWindowTileMapDisplaySelect = 1 << 6;
// Bit 7 - LCD Display Enable             (0=Off, 1=On)
unsigned char LCDCDisplayEnable = 1 << 7;

PPU::PPU(GameboyMem& gameboyMem, sf::Vector2u winSize) :
    display(sf::Image({160, 144}, sf::Color::Black)),
    displayTexture(display),
    displaySprite(displayTexture),
    oam(gameboyMem),
    fetcher(gameboyMem)
{
    background = std::vector<unsigned char>(256 * 256);
    window = std::vector<unsigned char>(256 * 256);
    viewport = std::vector<unsigned char>(160 * 144);
    fetcher.setup();

    graphicsData = &gameboyMem.read(0x8000);
    backgroundMap1 = &gameboyMem.read(0x9800);
    backgroundMap2 = &gameboyMem.read(0x9C00);
    OAMemory = &gameboyMem.read(0xFE00);

    SCY = &gameboyMem.mem[0xFF42];
    SCX = &gameboyMem.mem[0xFF43];
    LY = &gameboyMem.mem[0xFF44];
    LYC = &gameboyMem.mem[0xFF45];
    WX = &gameboyMem.mem[0xFF4B];
    WY = &gameboyMem.mem[0xFF4A];
    LCDC = &gameboyMem.mem[LCDC_ADDR];
    STAT = &gameboyMem.mem[STAT_ADDR];
    IF = &gameboyMem.mem[IF_ADDR];

    test = sf::RectangleShape({160, 144});
    test.setFillColor(sf::Color::Red);
    test.setOrigin({80, 72});
    //test.setScale({200.f, 200.f});
    test.setPosition({0.f, 0.f});

    displaySprite.setOrigin({80, 72});
    displaySprite.setScale({3.f, 3.f});

    sf::Vector2f midPoint = {(winSize.x / 2.f),
                            (winSize.y / 2.f)};
    displaySprite.setPosition(midPoint);

    state = OAMSearch;
}

void PPU::main() {
    // Check for interrupts
    unsigned char interrupt;
    
    //*STAT |= 0x1;

    // Instead, update mode bits based on state
    *STAT &= 0xFC;  // Clear mode bits (0-1)

    switch (state) {
        case OAMSearch:
            *STAT |= 0x02;  // Mode 2
            break;
        case PixelTransfer:
            *STAT |= 0x03;  // Mode 3
            break;
        case HBlank:
            *STAT |= 0x00;  // Mode 0
            break;
        case VBlank:
            *STAT |= 0x01;  // Mode 1
            break;
    }

    // Set LYC==LY status
    if (*LYC == *LY) {
        *STAT |= 0x04;
        if ((*STAT & STAT_LYC_INTERRUPT) != 0) {
            *IF |= 2; // SET LCD interrupt
        }
    } else {
        *STAT &= 0xFB; // 0b11111011
    }

    cycles++;

    PPUState oldState = state;

    switch (state) {
        case OAMSearch:
            OAMScan();
            break;
        case PixelTransfer:
            TransferPixels();
            break;
        case HBlank:
            DoHBlank();
            break;
        case VBlank:
            DoVBlank();
            break;
    }

    // Fire interrupts only on state transitions
    if (state != oldState) {
        switch (state) {
            case OAMSearch:
                if ((*STAT & STAT_MODE2_INTERRUPT) != 0) {
                    *IF |= 2;
                }
                break;
            case HBlank:
                if ((*STAT & STAT_MODE0_INTERRUPT) != 0) {
                    *IF |= 2;
                }
                break;
            case VBlank:
                *IF |= 0x01;  // VBlank interrupt
                if ((*STAT & STAT_MODE1_INTERRUPT) != 0) {
                    *IF |= 2;
                }
                break;
            default:
                break;
        }
    }
}

// H-Blank
// After drawing mode (mode 2)
void PPU::DoHBlank() {
    // TODO: pad timing to 456 T-Cycles
    // Enter VBlank if at end of screen
    if (cycles == 456) {
        cycles = 0;
        *LY = *LY + 1;
        if (*LY == 144) { 
            state = VBlank; 
            *IF |= 0x01;
        }
        else { 
            oam.start();
            state = OAMSearch;
        }
    }
}

// V-Blank
void PPU::DoVBlank() {
    // TODO: Takes place at end of every frame for 10 * 456 T-cycles
    
    // Special case for last line
    /*if (cycles == 4 && *LY == 153) {
        *LY = 0;
    }*/

    if (cycles == 456) {
        cycles = 0;
        if (*LY == 153) {
            *LY = 0;
            fetcher.vBufferIndex = 0;
            oam.start();
            readyToDraw = true;
            state = OAMSearch;
        } else {
            *LY = *LY + 1;
        }
    }
}

// OAM Scan
void PPU::OAMScan() {
    if (oam.tick()) {
        // Reset all objects
        for (auto& obj : oam.objects) {
            obj.fetched = false;
        }

        unsigned short int y = *SCY + *LY;
        unsigned short int tileLine = y % 8;
        unsigned short int baseAddr = 0x9800;
        unsigned char tileOffset = *SCX / 8;
        if ((*LCDC & LCDCBGTileMapDisplaySelect) != 0) {
            baseAddr = 0x9C00;
        }
        unsigned short int tileMapRowAddr = baseAddr + ((y / 8) * 32);
        unsigned short int tileDataAddr = 0x8000;
        bool signedId = false;
        if ((*LCDC & LCDCBGWindowTileDataSelect) == 0) {
            tileDataAddr = 0x9000;
            signedId = true;
        }

        fetcher.Start(tileMapRowAddr, tileDataAddr, tileOffset, tileLine, signedId);

        x = 0;
        pixelsToDrop = *SCX % 8;
        state = PixelTransfer;
    }
}

// Drawing mode
// Pixel Transfer
void PPU::TransferPixels() {
    // Fetch pixel data
    fetcher.Tick();

    if (IsFetchingSprites()) { return; }

    if (pixelsToDrop > 0 && fetcher.FIFO.size() > 8) {
        pixelsToDrop--;
        drop();
        return;
    }
    
    if (fetcher.FIFO.size() < 8) { return; }

    // Window
    if (!drawWindow && (*LCDC & LCDCWindowDisplayEnable) > 0 && 
            *LY >= *WY && x + 7 >= *WX) {
        drawWindow = true;
        pixelsToDrop = 0;

        // Reinitialize fetcher for window
        unsigned char y = *LY - *WY;
        unsigned char tileLine = y % 8;
        unsigned char tileOffset = (x - *WX + 7) / 8;
        unsigned short int baseAddr = 0x9800;
        if (*LCDC & LCDCWindowTileMapDisplaySelect) {
            baseAddr = 0x9C00;
        }
        unsigned short int tileMapRowAddr = baseAddr + ((y / 8) * 32);
        unsigned short int tileDataAddr = 0x8000;
        bool signedId = false;
        if (*LCDC & LCDCBGWindowTileDataSelect) {
            tileDataAddr = 0x9000;
            signedId = true;
        }
        fetcher.Start(tileMapRowAddr, tileDataAddr, tileOffset, tileLine, signedId);
        return;
    }

    // Sprites
    if ((*LCDC & LCDCSpriteDisplayEnable) != 0) {
        for (auto& obj : oam.objects) {
            if (obj.fetched) { 
                continue; 
            }

            bool fetch = false;
            unsigned char offset = 0;
            if (obj.xPos < 8 && x == 0) {
                fetch = true;
                offset = 8 - obj.xPos;
            } else if (obj.xPos - 8 == x) {
                fetch = true;
            } 

            if (fetch) {
                fetcher.FetchObject(obj, offset, *LY + 16 - obj.yPos);
                obj.fetched = true;
                return;
            }
        }
    }

    x++;
    fetcher.pushToVBuffer();

    if (x == 160) {
        drawWindow = false;
        state = HBlank;
    }
}

void PPU::drop() {
    fetcher.popFIFO();
}

void PPU::drawToScreen(sf::RenderWindow& win) {
    sf::Vector2u coords;

    display = sf::Image({160, 144}, reinterpret_cast<std::uint8_t*>(fetcher.videoBuffer.data()));

    bool worked = displayTexture.loadFromImage(display);
    if (!worked) {
        printf("Failed to load image to texture\n");
        exit(1);
    }
    displaySprite.setTexture(displayTexture);
    win.draw(displaySprite);
    printf("0x%2x\n", *LCDC);

    readyToDraw = false;
}

bool PPU::IsFetchingSprites() {
    return fetcher.state == ReadObjectID ||
        fetcher.state == ReadObjectFlags ||
        fetcher.state == ReadObjectData0 ||
        fetcher.state == ReadObjectData1 ||
        fetcher.state == MixFIFO;
}
