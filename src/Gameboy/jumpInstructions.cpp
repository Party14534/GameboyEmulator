#include "gameboy.h"

void Gameboy::relativeJump(Flag f, bool n) {
    char eChar = mem[PC];
    short int e = eChar;
    PC++;

    switch (f) {
        case Flag::ZF:
            if (r.zero == !n) {
                PC += e;
            }
            break;
        case Flag::CF:
            if (r.carry == !n) {
                PC += e;
            }
            break;
        case Flag::TF:
            PC += e;
            break;
        default:
            break;
    }
}
