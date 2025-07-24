#include "gameboy.h"

// 3 cycles
void Gameboy::loadToRegisterPair(RegisterPair target) {
    unsigned char lsb = mem[PC];
    PC++;
    unsigned short int msb = mem[PC];
    PC++;

    unsigned short int nn = (msb << 8) + lsb;

    switch (target) {
        case BC:
            r.setBC(nn);
            break;
        case DE:
            r.setDE(nn);
            break;
        case HL:
            r.setHL(nn);
            break;
        case RegisterPair::SP:
            SP = nn;
            break;
        default:
            break;
    }
}
