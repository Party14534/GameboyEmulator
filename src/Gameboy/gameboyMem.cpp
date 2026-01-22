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

    if (DOCTOR_LOGGING && addr == LY_ADDR) {
        mem[addr] = 0x90;
    }

    /*
    if (dmaActive && addr < 0xFF80) {
        static unsigned char dmaBlockedValue = 0xFF;
        dmaBlockedValue = 0xFF;

        // Allow I/O registers (0xFF00-0xFF7F) - PPU needs these!
        if (addr >= 0x8000 && addr < 0xFF80) {
            return mem[addr];
        }

        //printf("DMA blocked read at 0x%04x, PC=0x%04x\n", addr, *PC);
        //return dmaBlockedValue;  // Return 0xFF for blocked reads
    }*/

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

    switch (memType) {
        case MBC0:
            return (addr <= 0x7FFF) ? romMem[addr] : mem[addr];
            break;
        case MBC1:
            if (addr <= 0x3FFF) {
                // ROM Bank 00 (or X0 in mode 1)
                unsigned int bank = 0;
                if (bankingMode == 1) {
                    bank = upperBankBits << 5;
                }
                return romMem[(bank * 0x4000) + addr];
                
            } else if (addr <= 0x7FFF) {
                // ROM Bank 01-7F
                unsigned int bank = (upperBankBits << 5) | romBankLower;
                return romMem[(bank * 0x4000) + (addr - 0x4000)];
                
            } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                // RAM Bank 00-03
                if (!ramEnable) {
                    fakeVal = 0xFF;
                    return fakeVal;
                }
                unsigned int ramBank = (bankingMode == 1) ? upperBankBits : 0;
                return mem[(ramBank * 0x2000) + (addr - 0xA000) + 0xA000];
                
            } else {
                return mem[addr];
            };
        case MBC2:
            if (addr <= 0x3FFF) {
                // ROM Bank 0
                return romMem[addr];
                
            } else if (addr <= 0x7FFF) {
                // ROM Bank 01-0F
                unsigned int bank = romBankLower;
                return romMem[(bank * 0x4000) + (addr - 0x4000)];
                
            } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                // Built-in RAM (512 half-bytes with echoes)
                if (!ramEnable) {
                    fakeVal = 0xFF;
                    return fakeVal;
                }
                
                // Only bottom 9 bits of address are used (512 bytes)
                unsigned short int ramAddr = 0xA000 + (addr & 0x01FF);
                // Only lower 4 bits are valid, upper 4 are undefined
                mem[ramAddr] |= 0xF0; // TODO: This may be bad
                return mem[ramAddr];
                
            } else {
                return mem[addr];
            }
            break;
        case MBC3:
            if (addr <= 0x3FFF) {
                // ROM Bank 00
                return romMem[addr];
                
            } else if (addr <= 0x7FFF) {
                // ROM Bank 01-7F (reuse romBankLower)
                unsigned int bank = romBankLower;
                return romMem[(bank * 0x4000) + (addr - 0x4000)];
                
            } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                // RAM Bank 00-07 or RTC Register 08-0C
                if (!ramEnable) {  // Reuse ramEnable
                    fakeVal = 0xFF;
                    return fakeVal;
                }
                
                if (ramBankOrRTC <= 0x07) {
                    // RAM Bank access
                    if (!ramEnabled) {
                        fakeVal = 0xFF;
                        return fakeVal;
                    }
                    unsigned int ramBank = ramBankOrRTC;
                    return mem[(ramBank * 0x2000) + (addr - 0xA000) + 0xA000];
                    
                } else if (ramBankOrRTC >= 0x08 && ramBankOrRTC <= 0x0C) {
                    // RTC Register access (return latched values)
                    return rtcRegs[ramBankOrRTC - 0x08];
                    
                } else {
                    fakeVal = 0xFF;
                    return fakeVal;
                }
                
            } else {
                return mem[addr];
            }
            break;
        default:
            printf("Unsupported memory type\n");
            exit(1);
            break;
    }
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
    }

    // MBC1 register writes
    if (memType == MBC1 && addr < 0x8000) {
        if (addr <= 0x1FFF) {
            // RAM Enable
            if (!ramEnabled) return;
            ramEnable = ((val & 0x0F) == 0x0A) ? 1 : 0;
            
        } else if (addr <= 0x3FFF) {
            // ROM Bank Number (5-bit)
            romBankLower = val & 0x1F;
            if (romBankLower == 0) romBankLower = 1;  // 00→01 translation
            
        } else if (addr <= 0x5FFF) {
            // RAM Bank / Upper ROM bits
            upperBankBits = val & 0x03;
            
        } else if (addr <= 0x7FFF) {
            // Banking Mode Select
            bankingMode = val & 0x01;
        }
        return;
    }

    // MBC2 register writes
    if (memType == MBC2 && addr < 0x8000) {
        if (addr <= 0x3FFF) {
            // Check bit 8 of address (bit 0 of upper byte)
            if ((addr & 0x0100) == 0) {
                // Bit 8 is 0: RAM Enable
                // Examples: 0000-00FF, 0200-02FF, 0400-04FF, etc.
                ramEnable = ((val & 0x0F) == 0x0A) ? 1 : 0;
                
            } else {
                // Bit 8 is 1: ROM Bank Number (4-bit)
                // Examples: 0100-01FF, 0300-03FF, 0500-05FF, etc.
                romBankLower = val & 0x0F;
                if (romBankLower == 0) romBankLower = 1;  // 00→01 translation
            }
        }
        return;
    }

    // MBC3 register writes
    if (memType == MBC3 && addr < 0x8000) {
        if (addr <= 0x1FFF) {
            // RAM and Timer Enable (reuse ramEnable)
            ramEnable = ((val & 0x0F) == 0x0A) ? 1 : 0;
            
        } else if (addr <= 0x3FFF) {
            // ROM Bank Number (7-bit, reuse romBankLower)
            romBankLower = val & 0x7F;
            if (romBankLower == 0) romBankLower = 1;  // 00→01 translation
            
        } else if (addr <= 0x5FFF) {
            // RAM Bank Number or RTC Register Select
            ramBankOrRTC = val;
            
        } else if (addr <= 0x7FFF) {
            // Latch Clock Data (00->01 sequence)
            if (latchData == 0x00 && val == 0x01) {
                // Latch the RTC registers
                for (int i = 0; i < 5; i++) {
                    rtcRegs[i] = rtcRegsInternal[i];
                }
            }
            latchData = val;
        }
        return;
    }

    // Handle RAM writes
    if (memType == MBC1 && addr >= 0xA000 && addr <= 0xBFFF) {
        if (ramEnable && ramEnabled) {
            unsigned int ramBank = (bankingMode == 1) ? upperBankBits : 0;
            mem[(ramBank * 0x2000) + (addr - 0xA000) + 0xA000] = val;
        }
        return;
    }

    if (memType == MBC2 && addr >= 0xA000 && addr <= 0xBFFF) {
        if (ramEnable && ramEnabled) {
            // Only bottom 9 bits of address are used
            unsigned short int ramAddr = 0xA000 + (addr & 0x01FF);
            // Only lower 4 bits are stored
            mem[ramAddr] = val & 0x0F;
        }
        return;
    }
    // MBC3 RAM/RTC writes
    if (memType == MBC3 && addr >= 0xA000 && addr <= 0xBFFF) {
        if (ramEnable) {  // Reuse ramEnable
            if (ramBankOrRTC <= 0x07) {
                // RAM Bank write
                if (ramEnabled) {
                    unsigned int ramBank = ramBankOrRTC;
                    mem[(ramBank * 0x2000) + (addr - 0xA000) + 0xA000] = val;
                }
                
            } else if (ramBankOrRTC >= 0x08 && ramBankOrRTC <= 0x0C) {
                // RTC Register write
                unsigned char regIndex = ramBankOrRTC - 0x08;
                switch (regIndex) {
                    case 0: rtcRegsInternal[0] = val & 0x3F; break;  // Seconds: 0-59
                    case 1: rtcRegsInternal[1] = val & 0x3F; break;  // Minutes: 0-59
                    case 2: rtcRegsInternal[2] = val & 0x1F; break;  // Hours: 0-23
                    case 3: rtcRegsInternal[3] = val; break;         // Days low
                    case 4: rtcRegsInternal[4] = val & 0xC1; break;  // Days high (bits 0,6,7)
                }
            }
        }
        return;
    }

    if (*bootFinished && addr < 0x7FFF) {
        switch (memType) {
            case MBC0:
                return;
                break;
            default:
                printf("Code shouldn't be reached\n");
                exit(1);
        }
    }

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
