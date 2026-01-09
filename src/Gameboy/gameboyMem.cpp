#include "gameboy.h"

GameboyMem::GameboyMem(unsigned short int& _PC) {
    mem = std::vector<unsigned char>(0xFFFF + 1);
    bootRomMem = std::vector<unsigned char>(0xFF);
    bootFinished = &mem[0xFF50];
    PC = &_PC;
}

unsigned char& GameboyMem::read(unsigned short int addr) {
    if (!*bootFinished && addr <= 0xFF) {
        return bootRomMem[addr];
    }

    if (addr == IE_ADDR) {
        //printf("Trying to read IE\n");
    }

    if (DOCTOR_LOGGING && addr == LY_ADDR) {
        mem[addr] = 0x90;
    }

    if (addr == 0xFF00) {
        // Joypad handling
        unsigned char joypad = mem[addr];
        unsigned char buttons = 0x0F;  // Default: no buttons pressed (all 1s)

        if ((joypad & 0x20) == 0) {  // Button keys selected
            /*printf("Checking buttons: start=%d, select=%d, a=%d, b=%d\n", 
               startButton, selectButton, aButton, bButton);*/

            // Bit 3: Start, Bit 2: Select, Bit 1: B, Bit 0: A
            if (startButton) buttons &= ~0x08;   // Start (bit 3 = 0)
            if (selectButton) buttons &= ~0x04;  // Select (bit 2 = 0)
            if (bButton) buttons &= ~0x02;       // B (bit 1 = 0)
            if (aButton) buttons &= ~0x01;       // A (bit 0 = 0)

            mem[addr] = (joypad & 0xF0) | buttons;  // Combine selection bits with button states
            //printf("Joypad read! Returning: 0x%02x\n", mem[addr]);
        }
    }

    return mem[addr];
}

void GameboyMem::write(unsigned short int addr, unsigned char val) {
    mem[addr] = val;

    switch(addr) {
        case 0xFF01:
        case 0xFF02:
            if (LOG_SERIAL) {
                //printf("SERIAL: %04x %d\n", val, val);
            }
            break;
        case LCDC_ADDR:
            //printf("LCDC WRITTEN: 0x%02x\n", mem[addr]);
            break;
        case STAT_ADDR:
            //printf("STAT WRITTEN: 0x%02x\n", mem[addr]);
            break;
        /*case 0xFF50:
            if (val == 0) { return; }
            break;

            *PC = 0x100;
            return;*/
        default:
            break;
    }
    /*if (addr == 0xFF50 && val != 0) {
        *PC = 0x100;
    }*/
}
