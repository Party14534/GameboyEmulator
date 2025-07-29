#include "gameboy.h"

void Gameboy::relativeJump(Flag f) {
    char eChar = mem[PC];
    short int e = eChar;
    PC++;

    switch (f) {
        case Flag::ZF:
            if (!r.zero) {
                PC += e;
            }
            break;
        case Flag::CF:
            if (!r.carry) {
                PC += e;
            }
            break;
        default:
            break;
    }
}
