#include "gameboy.h"

GameboyMem::GameboyMem(unsigned short int& _PC, int& _cycles) {
    mem = std::vector<unsigned char>(0xFFFF + 1);
    bootRomMem = std::vector<unsigned char>(0xFF);
    bootFinished = &mem[0xFF50];
    PC = &_PC;
    cycles = &_cycles;
    dmaActive = false;
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

    if (addr == 0xFF80) {
        printf("Reading %02x from FF80\n", mem[addr]);
    }
    if (addr == 0xFF81) {
        printf("Reading %02x from FF81\n", mem[addr]);
    }

    if (dmaActive && addr < 0xFF80) {
        static unsigned char dmaBlockedValue = 0xFF;
        dmaBlockedValue = 0xFF;

        // Allow I/O registers (0xFF00-0xFF7F) - PPU needs these!
        if (addr >= 0x8000 && addr < 0xFF80) {
            return mem[addr];
        }

        //printf("DMA blocked read at 0x%04x, PC=0x%04x\n", addr, *PC);
        //return dmaBlockedValue;  // Return 0xFF for blocked reads
    }

    if (addr == 0xFF00) {
        // Joypad handling
        unsigned char joypad = mem[addr];
        unsigned char buttons = 0x0F;  // Default: no buttons pressed (all 1s)

        if ((joypad & 0x20) == 0) {  // Button keys selected
            // Bit 3: Start, Bit 2: Select, Bit 1: B, Bit 0: A
            if (startButton) buttons &= ~0x08;   // Start (bit 3 = 0)
            if (selectButton) buttons &= ~0x04;  // Select (bit 2 = 0)
            if (bButton) buttons &= ~0x02;       // B (bit 1 = 0)
            if (aButton) buttons &= ~0x01;       // A (bit 0 = 0)
        } else if ((joypad & 0x10) == 0) {  // Direction keys
            if (downButton) buttons &= ~0x08;
            if (upButton) buttons &= ~0x04;
            if (leftButton) buttons &= ~0x02;
            if (rightButton) buttons &= ~0x01;
        }

        unsigned char newState = (joypad & 0xF0) | buttons;  // Combine selection bits with button states

         // A bit going from 1 (not pressed) to 0 (pressed) triggers interrupt

        unsigned char pressed = prevJoypadState & ~newState;  // Bits that changed from 1 to 0
        if ((pressed & 0x0F) != 0) {
            // A button was pressed - request joypad interrupt
            mem[IF_ADDR] |= 0x10;  // Set bit 4 (joypad interrupt)
        }

        prevJoypadState = newState;
        mem[addr] = newState;
    }

    return mem[addr];
}

void GameboyMem::write(unsigned short int addr, unsigned char val) {
    if (addr == 0xFF46) {
        // Calculate source address (val * 0x100)
        unsigned short int source = ((unsigned short int)val) << 8;

        // Copy 160 bytes from source to OAM (0xFE00-0xFE9F)
        for (int i = 0; i < 160; i++) {
            mem[0xFE00 + i] = mem[source + i];
        }

        dmaActive = true;
        dmaCyclesRemaining += 160;  // Optional
        printf("starting dma transfer 0x%02x %04x\n", val, source);
    }

    if (addr == 0xFF80 && val == 0x2f) {
        printf("Writing %02x to FF80\n", val);
    }
    if (addr == 0xFF81 && val == 0x2f) {
        printf("Writing %02x to FF81\n", val);
    }

   /* if (addr >= 0xFE00 && addr <= 0xFE9F) {
        unsigned char stat = mem[STAT_ADDR];
        unsigned char mode = stat & 0x03;
        if (mode == 2 || mode == 3) {
            // OAM is locked during OAM Search and Pixel Transfer
            return;  // Ignore write
        }
    }

    // Protect VRAM during mode 3
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        unsigned char stat = mem[STAT_ADDR];
        unsigned char mode = stat & 0x03;
        if (mode == 3) {
            // VRAM is locked during Pixel Transfer
            return;  // Ignore write
        }
    }

    // TODO: Protect more I/O registers
    // Some I/O registers are read-only
    if (addr == 0xFF44) {  // LY is read-only
        return;
    }*/

    switch(addr) {
        case DIV_ADDR:
            mem[addr] = 0;
            break;
        default:
            mem[addr] = val;
            break;
    }

    /*if (addr == 0xFF50 && val != 0) {
        *PC = 0x100;
    }*/
}
