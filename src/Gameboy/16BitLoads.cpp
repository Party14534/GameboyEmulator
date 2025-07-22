#include "gameboy.h"

// 2 cycles
void Gameboy::incRegisterPair(RegisterPair pair, short int amount) {
    unsigned short int val;
    switch (pair) {
        case AF:
            printf("Error: Cannot inc AF");
            exit(1);
            break;
        case BC:
            val = r.getBC();
            val += amount;
            r.setBC(val);
            break;
        case DE:
            val = r.getDE();
            val += amount;
            r.setDE(val);
            break;
        case HL:
            val = r.getHL();
            val += amount;
            r.setHL(val);
            break;
        case RegisterPair::SP:
            SP += amount;
            break;
    }
}
