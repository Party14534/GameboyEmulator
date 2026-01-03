#include "gameboy.h"

#define SPRITE_SIZE 4

OAM::OAM(GameboyMem& _mem) : mem(_mem) {
    state = ReadSpriteY;
}

void OAM::start() {
    index = 0;
    state = ReadSpriteY;
    object = OAMObject{};
    //for (auto& obj : objects) { obj.fetched = false; }
    objects.clear();
}

bool OAM::tick() {
    unsigned short int addr = 0xFE00 + index * 4; // OAMMemory + index
    object.addr = addr;

    switch (state) {
        case ReadSpriteY:
            object.yPos = mem.read(addr);
            state = ReadSpriteX;
            break;

        case ReadSpriteX:
            // Sprite table must have room left
            if (objects.size() == 10) {
                return true;
            }

            object.xPos = mem.read(addr + 1);
            
            unsigned char height = 8;
            // (1 << 2) LCDCSpriteSize
            if ((mem.read(LCDC_ADDR) & (1 << 2)) != 0) {
                height = 16; 
            } 
            
            if (object.xPos != 0) {
                unsigned char y = mem.read(LY_ADDR) + 16;
                if (object.yPos <= y && object.yPos + height > y) {
                    object.fetched = false;
                    objects.push_back(object);
                }
            }

            state = ReadSpriteY;
            index++;
            break;
    }

    return index >= 40;
}
