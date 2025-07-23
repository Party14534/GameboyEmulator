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

// 2 cycles
void Gameboy::addRegisterPairs(RegisterPair target, RegisterPair source) {
    unsigned short int val;
    unsigned short int oldVal;

    switch (source) {
        case AF:
            val = r.getAF();
            break;
        case BC:
            val = r.getBC();
            break;
        case DE:
            val = r.getDE();
            break;
        case HL:
            val = r.getHL();
            break;
        case RegisterPair::SP:
            val = SP;
            break;
    }

    switch (target) {
        case HL:
            oldVal = r.getHL();
            break;
        case RegisterPair::SP:
            oldVal = SP;
            break;
        default:
            printf("Error: Can only add to HL and SP\n");
            exit(1);
    } 

    unsigned short int result = oldVal + val;

    r.subtract = 0;
    r.carry = (oldVal > result);
    r.halfCarry = (((oldVal & 0x0F) + (val & 0x0F)) > 0x0F);
}
