#include "gameboy.h"

// 2 cycles
void Gameboy::incRegisterPair(RegisterPair pair) {
    unsigned short int val;
    switch (pair) {
        case AF:
            printf("Error: Cannot inc AF");
            exit(1);
            break;
        case BC:
            val = r.getBC();
            val += 1;
            r.setBC(val);
            break;
        case DE:
            val = r.getDE();
            val += 1;
            r.setDE(val);
            break;
        case HL:
            val = r.getHL();
            val += 1;
            r.setHL(val);
            break;
        case RegisterPair::SP:
            SP += 1;
            break;
    }
}
