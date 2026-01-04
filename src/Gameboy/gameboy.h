#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>

#define LOGGING false
#define DOCTOR_LOGGING false
#define LOG_SERIAL false
#define LOGFLAGS false
#define WRITEHEADER true
#define LY_ADDR 0xFF44
#define LYC_ADDR 0xFF45
#define LCDC_ADDR 0xFF40
#define IF_ADDR 0xFF0F
#define IE_ADDR 0xFFFF

// STAT
#define STAT_ADDR 0xFF41
#define STAT_LYC_INTERRUPT 0x40
#define STAT_MODE0_INTERRUPT 0x20
#define STAT_MODE1_INTERRUPT 0x10
#define STAT_MODE2_INTERRUPT 0x08

// INTERRUPTS
#define VBLANK_INTERRUPT_VECTOR 0x0040
#define LCD_INTERRUPT_VECTOR 0x0048
#define TIMER_INTERRUPT_VECTOR 0x0050
#define SERIAL_INTERRUPT_VECTOR 0x0058
#define JOYPAD_INTERRUPT_VECTOR 0x0060

enum RegisterIndex {
    A = 0, B, C, D, E, F, H, L
};

enum RegisterPair {
    AF = 0, BC, DE, HL, SP
};

enum Flag {
    ZF, NF, HF, CF, TF
};

enum PPUState {
    OAMSearch,
    PixelTransfer,
    HBlank,
    VBlank
};

enum FetcherState {
    ReadTileID,
    ReadTileData0,
    ReadTileData1,
    PushToFIFO,
    ReadObjectID,
    ReadObjectFlags,
    ReadObjectData0,
    ReadObjectData1,
    MixFIFO
};

inline std::vector<sf::Color> paletteOne;

struct OAMProperties {
    bool priority;
    bool flipY;
    bool flipX;
    bool palette;
    bool bank;
};

struct OAMObject {
    unsigned char yPos;
    unsigned char xPos;
    unsigned char tileIndex;
    unsigned short int addr;
    OAMProperties attributes;
    bool fetched = false;
};

enum OAMState {
    ReadSpriteY,
    ReadSpriteX
};

struct GameboyMem {
    std::vector<unsigned char> mem;
    std::vector<unsigned char> bootRomMem;
    unsigned char* bootFinished;
    unsigned short int* PC;

    GameboyMem(unsigned short int& PC);
    unsigned char& read(unsigned short int addr);
    void write(unsigned short int addr, unsigned char val);
};

struct OAM {
    unsigned short int index;
    GameboyMem& mem;
    std::vector<OAMObject> objects;
    OAMObject object; // Current object
    OAMState state;
    OAM(GameboyMem& mem);
    
    bool tick();
    void start();
};

struct Pixel {
    unsigned char color;
    bool palette;
    bool priority; // only used for sprites
    bool isObject = false;
};

struct Fetcher {
    std::vector<Pixel> FIFO;
    std::vector<unsigned char> tileData;
    std::vector<unsigned char> objectData;
    std::vector<sf::Color> videoBuffer;
    GameboyMem& mem;
    FetcherState state;
    FetcherState oldState;
    OAMObject object;
    int cycles;
    unsigned short int mapAddr;
    unsigned short int dataAddr;
    unsigned short int tileLine;
    unsigned short int tileID;
    bool signedId;
    unsigned char tileOffset;
    unsigned char objectOffset;
    unsigned char objectLine;
    unsigned char objectId;
    unsigned int vBufferIndex = 0;

    unsigned char* BGP;
    unsigned char* OBP0;
    unsigned char* OBP1;

    Fetcher(GameboyMem& _mem);

    void setup();
    void Start(unsigned short int mapAddr, unsigned short int dataAddr, unsigned char tileOffset, unsigned short int tileLine, bool signedId);
    void FetchObject(OAMObject object, unsigned char offset, unsigned char line);
    void Tick();
    void readTileData(unsigned short int addrOffset);
    void readObjectData(unsigned short int addrOffset);
    void MixInFifo();
    void pushToFIFO();
    void popFIFO();
    void pushToVBuffer();
};

struct PPU {
    std::vector<unsigned char> viewport;
    std::vector<unsigned char> background;
    std::vector<unsigned char> window;

    std::vector<Pixel> spriteFIFO;
    std::vector<Pixel> bgWinFIFO;
    
    // Pointers to VRAM sections
    unsigned char* graphicsData; // 8000 - 97FF
    
    // Hold data for background tiles layed out row by row.
    // Each byte is a tile number and is on the background buffer
    // based on where it is in the graphics data. E.g. the first
    // byte would be in the top left corner and the 33rd would be
    // placed in the leftmost position of the second row.
    unsigned char* backgroundMap1; // 9800 - 9BFF
    unsigned char* backgroundMap2; // 9C00 - 9FFF
    
    // Contains data for displaying sprites where each sprite
    // takes up 4 bytes
    // Byte 0: Y pos
    // Byte 1: X pos
    // Byte 2: Tile number Always using 8000 addressing mode
    // Byte 3: Sprite flags
    //          Bit 7:  0 = sprite always rendered above bg
    //                  1 = Bg colors 1-3 overlay sprite, sprite
    //                      is still rendered above color 0
    //          Bit 6:  Y flip
    //          Bit 5:  X flip
    //          Bit 4: Palette Number
    unsigned char* OAMemory; // FE00 - FE9F

    // Register pointers
    unsigned char* SCY; // FF42
    unsigned char* SCX; // FF43
    unsigned char* LCDC; // FF40
    unsigned char* STAT; // FF41
    unsigned char* WY; // FF4A
    unsigned char* WX; // FF4B
    unsigned char* IF; // FF4B
    unsigned char* LY = 0; // Line currently being displayed
    unsigned char* LYC = 0; // Used to set STAT interrupts

    PPUState state = OAMSearch;
    unsigned short int cycles = 0; // T-Cycles for current line
    unsigned short int x = 0; // Num pixels already output in current line
    unsigned short int pixelsToDrop = 0;
    unsigned short int windowLineCounter = 0;

    Fetcher fetcher;
    OAM oam;
    sf::Image display;
    sf::Texture displayTexture;
    sf::Sprite displaySprite;
    sf::RectangleShape test;
    bool readyToDraw = false;
    bool drawWindow = false;
    
    PPU(GameboyMem& ameboyMem, sf::Vector2u winSize);
    void main();

    void DoHBlank();
    void DoVBlank();
    void OAMScan();
    void TransferPixels();
    bool IsFetchingSprites();

    void drop();

    void drawToScreen(sf::RenderWindow& win);
};

struct Registers {
    std::vector<unsigned char> registers;

    // Flags register
    bool zero = false;
    bool subtract = false;
    bool halfCarry = false;
    bool carry = false;
    bool modifiedFlags = false;

    void setFlags();
    void setF();

    void setAF(short unsigned int val);
    void setBC(short unsigned int val);
    void setDE(short unsigned int val);
    void setHL(short unsigned int val);

    short unsigned int getAF();
    short unsigned int getBC();
    short unsigned int getDE();
    short unsigned int getHL();

    unsigned char fRtoU8();
    void u8toFR(unsigned char byte);
};


struct Gameboy {
    /*
     * Variable Definitions
     */
    short unsigned int PC = 0; // Program counter
    short unsigned int SP = 0; // Stack pointer
    short unsigned int IX = 0; // Index register
    short unsigned int IY = 0; // Index register
    // unsigned char A = 0; // 8-bit accumulator
    unsigned char I = 0; // Interrupt Page Address register
    unsigned char R = 0; // Memory Refresh register
    unsigned char* IE = 0; // Interrupt Enable 0xFFFF
    bool IME = false;
    bool testing = false;
    Registers r;

    unsigned short int cycles;

    // Emulator variables
    std::string romPath;
    GameboyMem mem;
    PPU ppu;

    /*
     * Function Definitions
     */
    Gameboy(std::string _romPath, sf::Vector2u winSize = {160, 144}, bool bootRom = false, bool _testing = false);
    void writeBootRom();
    void writeRom();
    void FDE();
    unsigned char fetch();
    void decode(unsigned char instruction);

    RegisterIndex byteToIndex(unsigned char secondHalfByte);

    void call0XInstructions(unsigned char secondHalfByte);
    void call1XInstructions(unsigned char secondHalfByte);
    void call2XInstructions(unsigned char secondHalfByte);
    void call3XInstructions(unsigned char secondHalfByte);
    void call4X6XInstructions(RegisterIndex target, unsigned char secondHalfByte);
    void call7XInstructions(unsigned char secondHalfByte);
    void call8XInstructions(unsigned char secondHalfByte);
    void call9XInstructions(unsigned char secondHalfByte);
    void callAXInstructions(unsigned char secondHalfByte);
    void callBXInstructions(unsigned char secondHalfByte);
    void callCXInstructions(unsigned char secondHalfByte);
    void callDXInstructions(unsigned char secondHalfByte);
    void callEXInstructions(unsigned char secondHalfByte);
    void callFXInstructions(unsigned char secondHalfByte);

    // Instructions

    // Jump instructions
    void relativeJump(Flag f, bool n);

    // 0x4-7
    void load(RegisterIndex target, RegisterIndex value);
    void loadFromMemory(RegisterIndex target);
    void loadToMemory(RegisterIndex value);

    // 0x8
    void add(RegisterIndex target, bool carry);
    void addFromMemory(bool carry);

    // 0x9
    void subtract(RegisterIndex target, bool carry);
    void subtractFromMemory(bool carry);

    // 0xA
    void bitwiseAnd(RegisterIndex target);
    void bitwiseAndImmediate();
    void bitwiseAndFromMemory();
    void bitwiseXor(RegisterIndex target);
    void bitwiseXorFromMemory();
    void bitwiseXorImmediate();

    // 0xB
    void bitwiseOr(RegisterIndex target);
    void bitwiseOrFromMemory();
    void compare(RegisterIndex target);
    void compareFromMemory();
    void compareN();

    // 8bit loads
    void loadToRegister(RegisterIndex target);
    void loadImmediateDataToMemory();
    void loadFromAcc(bool usingC);
    void loadFromAcc(RegisterPair src, bool inc);
    void loadFromStack();
    void loadToAcc(bool usingC);
    void loadToAcc(RegisterPair target, short int change);
    void loadAccToMemory();
    void loadMemoryToAcc();

    // 8bit arithmetic
    void incRegister(RegisterIndex target, char val);
    void incMemory(char val);
    void addImmediate(bool carry);
    void subtractImmediate(bool carry);
    void bitwiseOrImmediate();
    void addEToSP();

    // 16bit arithmetic
    void incRegisterPair(RegisterPair pair, short int val);
    void addRegisterPairs(RegisterPair target, RegisterPair source);
    void addImmediateAndSPToHL();

    // 16bit loads
    void loadToRegisterPair(RegisterPair target);
    void popToRegisterPair(RegisterPair target);
    void pushRegisterPair(RegisterPair target);

    // Control Flow
    void callFunction();
    void callNN(Flag flag, bool notFlag);
    void jumpNN(std::optional<Flag> flag, bool notFlag);
    void ret(std::optional<Flag> flag, bool notFlag);
    void restart(unsigned char addr);

    // MISC
    void DAA();

    // CB
    void loadCBInstruction();
    RegisterIndex operandToIndex(unsigned char op);
    void rotateRegisterLeft(RegisterIndex target);
    void rotateRegisterRight(RegisterIndex target);
    void rotateRegisterLeftHL();
    void rotateRegisterRightHL();

    void RLC(RegisterIndex target);
    void RRC(RegisterIndex target);
    void RLCHL();
    void RRCHL();
    void bit(RegisterIndex target, unsigned short int bitOffset);
    void swap(RegisterIndex target);
    void srl(RegisterIndex target);
    void resetBit(RegisterIndex target, unsigned short bitOffset);
    void setBit(RegisterIndex target, unsigned short bitOffset);
};

#endif
