#include "gameboy.h"

void Gameboy::loadCBInstruction() {
    // Load next byte
    unsigned char byte = mem[PC];
    PC++;

    unsigned char twoBits = (byte & 0b11000000) >> 6;
    unsigned char fiveBits = (byte & 0b11111000) >> 3;
    unsigned char op = byte & 0b00000111;
    unsigned char bitIndex = (byte & 0b00111000) >> 3;

    if (twoBits != 0) {
        switch (twoBits) {
            case 0b00000001:
                break;
            case 0b00000010:
                break;
            case 0b00000011:
                break;
        }
    }
}
