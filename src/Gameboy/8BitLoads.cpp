#include "gameboy.h"

void Gameboy::load(RegisterIndex target, RegisterIndex value) {
    if (LOGGING) printf("LOAD REGISTER %c TO REGISTER %c\n", value + 65, target + 65);
    r.registers[target] = r.registers[value];
}

// Two cycles
void Gameboy::loadFromMemory(RegisterIndex target) {
    unsigned short int addr = r.registers[RegisterIndex::L];
    
    addr += (r.registers[RegisterIndex::H] << 8);
    unsigned char value = mem.read(addr);

    if (LOGGING) printf("LOAD %d FROM ADDR: 0x%04x TO REGISTER %c\n", value, addr, target + 65);
    r.registers[target] = value;
}

// Two cycles
// 0x70 - 0x75, 0x77
void Gameboy::loadToMemory(RegisterIndex value) {
    unsigned short int addr = r.getHL();

    if (LOGGING) printf("LOAD REGISTER %c TO ADDR: 0x%04x FROM HL\n", value + 65, addr);
    mem.write(addr, r.registers[value]);
}

// Two cycles
void Gameboy::loadToRegister(RegisterIndex target) {
    unsigned char data = mem.read(PC);
    PC++;
    
    if (LOGGING) printf("LOAD %d FROM ADDR: 0x%04X TO REGISTER %c\n", data, PC - 1, target + 65);
    r.registers[target] = data;
}

// 3 cycles
void Gameboy::loadImmediateDataToMemory() {
    unsigned char data = mem.read(PC);
    PC++;

    if (LOGGING) printf("LOAD %d FROM ADDR: 0x%04X TO HL\n", data, PC - 1);
    mem.write(r.getHL(), data);
}

// 2 cycles
void Gameboy::loadFromAcc(bool usingC) {
    unsigned short int addr = 0xFF00; 
    unsigned char n = (usingC) ? r.registers[C] : mem.read(PC);

    if (!usingC) { PC++; }
    addr |= n;

    mem.write(addr, r.registers[A]);

    if (LOGGING) printf("LOAD REGISTER A (0x%02x) TO ADDR: 0x%04x\n", r.registers[A], addr);
    //printf("%04x\n", addr);
}

// 2 cycles
void Gameboy::loadToAcc(bool usingC) {
    unsigned short int addr = 0xFF00; 
    unsigned char n = (usingC) ? r.registers[C] : mem.read(PC);

    if (!usingC) { PC++; }
    addr |= n;

    r.registers[A] = mem.read(addr);

    if (LOGGING) printf("LOAD ADDR: 0x%04x TO REGISTER A\n", addr);
    //printf("%04x\n", addr);
}

// 2 cycles
void Gameboy::loadToAcc(RegisterPair target, short int change) {
    unsigned short int addr;

    switch (target) {
        case BC:
            addr = r.getBC();
            break;
        case DE:
            addr = r.getDE();
            break;
        case HL:
            addr = r.getHL();
            r.setHL(addr + change);
            break;
        default:
            printf("Error: load to acc\n");
            exit(1);
            break;
    }

    r.registers[A] = mem.read(addr);
    if (LOGGING) printf("LOAD ADDR: 0x%04x TO REGISTER A FROM REGISTER PAIR\n", addr);
}

void Gameboy::loadFromAcc(RegisterPair src, bool inc) {
    unsigned short int addr;

    switch (src) {
        case BC:
            addr = r.getBC();
            break;
        case DE:
            addr = r.getDE();
            break;
        case HL:
            addr = r.getHL();
            break;
        default:
            printf("Register pair not set to valid pair\n");
            exit(1);
            break;
    }

    mem.write(addr, r.registers[RegisterIndex::A]);

    if (LOGGING) printf("LOAD REGISTER A (0x%02x) TO ADDR: 0x%04x\n", r.registers[A], addr);
    
    if (src == HL) {
        if (inc) { 
            addr++; 
        } else { 
            addr--; 
        }
        
        r.setHL(addr);
    }
}

// 4 cycles
void Gameboy::loadAccToMemory() {
    unsigned char lsb = mem.read(PC);
    PC++;

    unsigned char msb = mem.read(PC);
    PC++;

    unsigned short int addr = msb;
    addr = (addr << 8) | lsb;

    mem.write(addr, r.registers[A]);

    if (LOGGING) printf("LOAD REGISTER A TO ADDR: 0x%04x\n", addr);
}

// 4 cycles
void Gameboy::loadMemoryToAcc() {
    unsigned char lsb = mem.read(PC);
    PC++;

    unsigned char msb = mem.read(PC);
    PC++;

    unsigned short int addr = msb;
    addr = addr << 8;
    addr |= lsb;

    r.registers[A] = mem.read(addr);

    if (LOGGING) printf("LOAD ADDR: 0x%04x DATA: 0x%02x TO REGISTER A\n", addr, mem.mem[addr]);
}
