#include "gameboy.h"

PPU::PPU(GameboyMem& gameboyMem, sf::Vector2u winSize) :
    display(sf::Image({160, 144}, sf::Color::Black)),
    displayTexture(display),
    displaySprite(displayTexture),
    fetcher(gameboyMem)
{
    background = std::vector<unsigned char>(256 * 256);
    window = std::vector<unsigned char>(256 * 256);
    viewport = std::vector<unsigned char>(160 * 144);
    fetcher.setup();

    graphicsData = &gameboyMem[0x8000];
    backgroundMap1 = &gameboyMem[0x9800];
    backgroundMap2 = &gameboyMem[0x9C00];
    OAMemory = &gameboyMem[0xFE00];

    SCY = &gameboyMem[0xFF42];
    SCX = &gameboyMem[0xFF43];
    LY = &gameboyMem[0xFF44];
    WX = &gameboyMem[0xFF4B];
    WY = &gameboyMem[0xFF4A];
    LCDC = &gameboyMem[0xFF40];
    STAT = &gameboyMem[0xFF41];

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
    cycles++;
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
        *LY = *LY + 1;
        if (*LY == 144) { state = VBlank; }
        else { state = OAMSearch; }
    }
}

// V-Blank
void PPU::mode1() {
    // TODO: Takes place at end of every frame for 10 * 456 T-cycles
    if (cycles == 456) {
        cycles = 0;
        *LY = *LY + 1;
        if (*LY == 153) {
            readyToDraw = true;
            *LY = 0;
            state = OAMSearch;
        }
    }
}

// OAM Scan
void PPU::mode2() {
    // TODO: wait for 80 T-Cycles
    if (cycles == 80) {
        x = 0;
        unsigned short int y = *SCY + *LY;
        unsigned short int tileLine = y % 8;
        unsigned short int tileMapRowAddr = 0x9800 + ((y / 8) * 32);
        fetcher.Start(tileMapRowAddr, tileLine);
        state = PixelTransfer;
    }
}

// Drawing mode
void PPU::mode3() {
    // Fetch pixel data
    fetcher.Tick();
    
    if (fetcher.FIFO.size() <= 8) { return; }

    fetcher.pushToVBuffer();
    x++;

    if (x == 160) {
        state = HBlank;
    }
}

void PPU::drawToScreen(sf::RenderWindow& win) {
    sf::Vector2u coords;

    // TODO: test which is faster
    display = sf::Image({160, 144}, reinterpret_cast<std::uint8_t*>(fetcher.videoBuffer.data()));
    /*
    for (int i = 0; i < 160 * 144; i++) {
        coords.x = i % 160;
        coords.y = i / 160;
        display.setPixel(coords, fetcher.videoBuffer[i]);
    }*/

    bool worked = displayTexture.loadFromImage(display);
    if (!worked) {
        printf("Failed to load image to texture\n");
        exit(1);
    }
    displaySprite.setTexture(displayTexture);
    win.draw(displaySprite);

    readyToDraw = false;
}
