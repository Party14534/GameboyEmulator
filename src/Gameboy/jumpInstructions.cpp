#include "gameboy.h"

void Gameboy::relativeJump(Flag f, bool n) {
    char eChar = mem.read(PC);
    short int e = eChar;
    PC++;

    if (LOGGING) printf("JUMP %d: ", e);

    switch (f) {
        case Flag::ZF:
            if (LOGGING) printf("ZF ");
            if (r.zero == !n) {
                PC += e;
                cycles = 3;
                if (LOGGING) printf("JUMPED ");
            }
            break;
        case Flag::CF:
            if (LOGGING) printf("CF ");
            if (r.carry == !n) {
                PC += e;
                cycles = 3;
                if (LOGGING) printf("JUMPED ");
            }
            break;
        case Flag::TF:
            if (LOGGING) printf("TF JUMPED ");
            PC += e;
            cycles = 3;
            break;
        default:
            break;
    }

    if (LOGGING) printf("PC NOW: 0x%02x\n", PC);
    cycles = 2;
}
