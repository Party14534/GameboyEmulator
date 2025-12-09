#include "gameboy.h"

void Gameboy::callFunction() {
    unsigned char lsb = mem.read(PC);
    PC++;

    unsigned char msb = mem.read(PC);
    PC++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    unsigned char msbPC = (PC >> 8);
    unsigned char lsbPC = PC & 0x00FF;

    SP--;
    mem.write(SP, msbPC);
    SP--;
    mem.write(SP, lsbPC);

    if (LOGGING) printf("CALL FUNCTION: SET PC TO ADDR 0x%04x\n", addr);

    PC = addr;
}

void Gameboy::callNN(Flag flag, bool notFlag) {
    unsigned char lsb = mem.read(PC);
    PC++;

    unsigned char msb = mem.read(PC);
    PC++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    if (LOGGING) printf("CALL FUNCTION: ");
    switch (flag) {
        case Flag::CF:
            if (r.carry == notFlag) {
                if (LOGGING) printf("RET FUNCTION: C DID NOT RETURN\n");
                return;
            }
            break;
        case Flag::ZF:
            if (r.zero == notFlag) {
                if (LOGGING) printf("RET FUNCTION: Z DID NOT RETURN\n");
                return;
            }
            break;
        default:
            printf("Error: cannot compare flag that is not Z or C\n");
            exit(1);
            break;
    }

    unsigned char msbPC = (PC >> 8);
    unsigned char lsbPC = PC & 0x00FF;

    SP--;
    mem.write(SP, msbPC);
    SP--;
    mem.write(SP, lsbPC);

    if (LOGGING) printf("SET PC TO ADDR 0x%04x\n", addr);
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

    unsigned char lsb = mem.read(SP);
    SP++;

    unsigned char msb = mem.read(SP);
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
    mem.write(SP, PC >> 8); // MSB
    SP--;
    mem.write(SP, PC & 0x00FF); // LSB

    PC = addr;
}

// 4 cycles
void Gameboy::jumpNN(std::optional<Flag> flag, bool notFlag) {
    if (LOGGING) printf("JUMP FUNCTION: ");
    if (flag.has_value()) {
        switch (flag.value()) {
            case ZF:
                if (r.zero == notFlag) {
                    if (LOGGING) printf("JUMP FUNCTION: Z DID NOT RETURN\n");
                    return;
                }
                break;
            case CF:
                if (r.carry == notFlag) {
                    if (LOGGING) printf("JUMP FUNCTION: C DID NOT RETURN\n");
                    return;
                }
                break;
            default:
                printf("Unhandled return flag\n");
                exit(1);
                break;
        }
    }

    unsigned char lsb = mem.read(PC);
    PC++;
    unsigned char msb = mem.read(PC);
    PC++;
    
    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    PC = addr;
}
