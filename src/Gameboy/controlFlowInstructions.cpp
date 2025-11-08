#include "gameboy.h"

void Gameboy::callFunction() {
    unsigned char lsb = mem[PC];
    PC++;

    unsigned char msb = mem[PC];
    PC++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    unsigned char msbPC = (PC >> 8);
    unsigned char lsbPC = PC & 0x00FF;

    SP--;
    mem[SP] = msbPC;
    SP--;
    mem[SP] = lsbPC;

    if (LOGGING) printf("CALL FUNCTION: SET PC TO ADDR 0x%04x\n", addr);

    PC = addr;
}

void Gameboy::ret(std::optional<Flag> flag, bool notFlag) {
    if (LOGGING) printf("RET FUNCTION: ");
    if (flag.has_value()) {
        switch (flag.value()) {
            case ZF:
                if (r.zero == notFlag) {
                    if (LOGGING) printf("RET FUNCTION: Z DID NOT RETURN\n");
                    return;
                }
                break;
            case CF:
                if (r.carry == notFlag) {
                    if (LOGGING) printf("RET FUNCTION: C DID NOT RETURN\n");
                    return;
                }
                break;
            default:
                printf("Unhandled return flag\n");
                exit(1);
                break;
        }
    }

    unsigned char lsb = mem[SP];
    SP++;

    unsigned char msb = mem[SP];
    SP++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    PC = addr;

    if (LOGGING) printf("SET PC TO ADDR 0x%04x\n", addr);
}

// 4 cycles
void Gameboy::restart(unsigned char addr) {
    SP--;
    mem[SP] = PC >> 8; // MSB
    SP--;
    mem[SP] = PC & 0x00FF; // LSB

    PC = addr;
}
