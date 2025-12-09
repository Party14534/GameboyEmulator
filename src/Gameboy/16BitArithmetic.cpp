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
            if (LOGGING) printf("INC REGISTER PAIR BC BY %d\n", amount);
            val = r.getBC();
            val += amount;
            r.setBC(val);
            break;
        case DE:
            if (LOGGING) printf("INC REGISTER PAIR DE BY %d\n", amount);
            val = r.getDE();
            val += amount;
            r.setDE(val);
            break;
        case HL:
            if (LOGGING) printf("INC REGISTER PAIR HL BY %d\n", amount);
            val = r.getHL();
            val += amount;
            r.setHL(val);
            break;
        case RegisterPair::SP:
            if (LOGGING) printf("INC SP BY %d\n", amount);
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
            if (LOGGING) printf("ADD AF ");
            val = r.getAF();
            break;
        case BC:
            if (LOGGING) printf("ADD BC ");
            val = r.getBC();
            break;
        case DE:
            if (LOGGING) printf("ADD DE ");
            val = r.getDE();
            break;
        case HL:
            if (LOGGING) printf("ADD HL ");
            val = r.getHL();
            break;
        case RegisterPair::SP:
            if (LOGGING) printf("ADD SP ");
            val = SP;
            break;
    }

    switch (target) {
        case HL:
            if (LOGGING) printf("TO HL\n");
            oldVal = r.getHL();
            break;
        case RegisterPair::SP:
            if (LOGGING) printf("TO SP\n");
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
    r.modifiedFlags = true;

    r.setHL(result);
}

void Gameboy::addImmediateAndSPToHL() {
    signed char e = mem.read(PC);
    PC++;
    
    unsigned short int oldVal = SP;
    unsigned short int result = SP + e;
    
    r.zero = 0;
    r.subtract = 0;
    r.halfCarry = (((oldVal & 0x0F) + (e & 0x0F)) > 0x0F);
    r.carry = (oldVal > result);
    r.modifiedFlags = true;

    r.setHL(result);
}
