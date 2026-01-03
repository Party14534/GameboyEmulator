#include "gameboy.h"

// 3 cycles
void Gameboy::loadToRegisterPair(RegisterPair target) {
    unsigned char lsb = mem.read(PC);
    PC++;
    unsigned short int msb = mem.read(PC);
    PC++;

    unsigned short int nn = (msb << 8) + lsb;

    switch (target) {
        case BC:
            if (LOGGING) printf("LOAD %d to BC from 0x%04x\n", nn, PC - 2);
            r.setBC(nn);
            break;
        case DE:
            if (LOGGING) printf("LOAD %d to DE from 0x%04x\n", nn, PC - 2);
            r.setDE(nn);
            break;
        case HL:
            if (LOGGING) printf("LOAD %d to HL from 0x%04x\n", nn, PC - 2);
            r.setHL(nn);
            break;
        case RegisterPair::SP:
            if (LOGGING) printf("LOAD %d to SP from 0x%04x\n", nn, PC - 2);
            SP = nn;
            break;
        default:
            break;
    }
}

// 3 cycles
void Gameboy::popToRegisterPair(RegisterPair target) {
    unsigned char lsb = mem.read(SP);
    SP++;
    unsigned short int msb = mem.read(SP);
    SP++;

    unsigned short int nn = (msb << 8) + lsb;

    switch (target) {
        case BC:
            if (LOGGING) printf("SET BC TO %d FROM 0x%04x\n", nn, SP - 2);
            r.setBC(nn);
            break;
        case DE:
            if (LOGGING) printf("SET DE TO %d FROM 0x%04x\n", nn, SP - 2);
            r.setDE(nn);
            break;
        case HL:
            if (LOGGING) printf("SET HL TO %d FROM 0x%04x\n", nn, SP - 2);
            r.setHL(nn);
            break;
        case AF:
            if (LOGGING) printf("SET AF TO %d FROM 0x%04x\n", nn, SP - 2);
            r.setAF(nn);
            r.setFlags();
            break;
        default:
            break;
    }
}

void Gameboy::pushRegisterPair(RegisterPair target) {
    unsigned short int data;

    switch (target) {
        case BC:
            data = r.getBC();
            if (LOGGING) printf("PUSH BC: %d TO STACK\n", data);
            break;
        case DE:
            data = r.getDE();
            if (LOGGING) printf("PUSH DE: %d TO STACK\n", data);
            break;
        case HL:
            data = r.getHL();
            if (LOGGING) printf("PUSH HL: %d TO STACK\n", data);
            break;
        case AF:
            if (LOGGING) printf("PUSH AF: %d TO STACK\n", data);
            data = r.getAF();
            break;
        default:
            printf("Cannot use SP with this function\n");
            exit(1);
            break;
    }
    
    SP--;
    unsigned char msb = data >> 8;
    mem.write(SP, msb);
    
    SP--;
    unsigned char lsb = data & 0x00FF;
    mem.write(SP, lsb);
}

// 5 cycles
void Gameboy::loadFromStack() {
    unsigned char lsb = mem.read(PC);
    PC++;
    unsigned char msb = mem.read(PC);
    PC++;

    unsigned short int addr = msb << 8;
    addr |= lsb;

    unsigned char dataLsb = SP & 0x00FF;
    unsigned char dataMsb = (SP & 0xFF00) >> 8;

    mem.write(addr, dataLsb);
    mem.write(addr+1, dataMsb);

    if (LOGGING) printf("LOAD STACK TO ADDR: 0x%04x\n", addr);
}
