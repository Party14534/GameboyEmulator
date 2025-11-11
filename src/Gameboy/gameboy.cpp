#include "gameboy.h"
#include <optional>

Gameboy::Gameboy(std::string _romPath, sf::Vector2u winSize) : 
    romPath(_romPath),
    mem(), // 65535
    ppu(mem, winSize)
{
    r.registers = std::vector<unsigned char>(8);
    
    writeBootRom();
    writeRom();

    IE = &mem[0xFFFF];

    paletteOne = std::vector<sf::Color>{
        sf::Color(155, 188, 15),
        sf::Color(139, 172, 15),
        sf::Color(48, 98, 48),
        sf::Color(15, 56, 15)
    };
}

void Gameboy::writeBootRom() {
    std::ifstream file ("../src/res/dmg_boot.bin", std::ios::binary);
    printf("%ld\n", file.gcount());
    

    // Read entire file into vector
    mem.mem.assign(std::istreambuf_iterator<char>(file),
               std::istreambuf_iterator<char>());

    // Write cartridge header
    if (!WRITEHEADER) return;

    unsigned char header[] = {
        0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e
    };
    
    for (int i = 0x104; i <= 0x133; i++) {
        mem[i] = header[i - 0x104];
    }
}

void Gameboy::writeRom() {
    std::ifstream file ("../tests/cpu_instrs.gb", std::ios::binary);
    std::stringstream buff;
    buff << file.rdbuf();

    int i = 0x0;
    for (unsigned char byte : buff.str()) {
        mem[i] = byte;
        i++;
    }
    printf("%d\n", i);
}

void Gameboy::FDE() {
    unsigned char instruction = fetch();

    decode(instruction);

    if (r.modifiedFlags) { r.setF(); }
    r.modifiedFlags = false;

    if(LOGFLAGS && LOGGING) printf("zero: %d, sub: %d, half: %d, carry: %d\n",
            r.zero, r.subtract, r.halfCarry, r.carry);
}

unsigned char Gameboy::fetch() {
    unsigned char instruction = mem[PC];

    PC++;

    return instruction;
}

void Gameboy::decode(unsigned char instruction) {
    if(LOGGING) printf("---\nINSTRUCTION: %02x | PC: %d 0x%04x\n", instruction, PC - 1, PC - 1);
    unsigned char firstHalfByte = instruction & 0xF0;
    unsigned char secondHalfByte = instruction & 0x0F;


    switch (firstHalfByte) {
        case 0x00:
            call0XInstructions(secondHalfByte);
            break;
        case 0x10:
            call1XInstructions(secondHalfByte);
            break;
        case 0x20:
            call2XInstructions(secondHalfByte);
            break;
        case 0x30:
            call3XInstructions(secondHalfByte);
            break;
        case 0x40:
            call4X6XInstructions(RegisterIndex::B, secondHalfByte);
            break;
        case 0x50:
            call4X6XInstructions(RegisterIndex::D, secondHalfByte);
            break;
        case 0x60:
            call4X6XInstructions(RegisterIndex::H, secondHalfByte);
            break;
        case 0x70:
            call7XInstructions(secondHalfByte);
            break;
        case 0x80:
            call8XInstructions(secondHalfByte);
            break;
        case 0x90:
            call9XInstructions(secondHalfByte);
            break;
        case 0xA0:
            callAXInstructions(secondHalfByte);
            break;
        case 0xB0:
            callBXInstructions(secondHalfByte);
            break;
        case 0xC0:
            callCXInstructions(secondHalfByte);
            break;
        case 0xD0:
            callDXInstructions(secondHalfByte);
            break;
        case 0xE0:
            callEXInstructions(secondHalfByte);
            break;
        case 0xF0:
            callFXInstructions(secondHalfByte);
            break;
        default:
            printf("Unknown instruction %04x\n", instruction);
            exit(1);
    }
}

RegisterIndex Gameboy::byteToIndex(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
        case 0x08:
            return RegisterIndex::B;
        case 0x01:
        case 0x09:
            return RegisterIndex::C;
        case 0x02:
        case 0x0A:
            return RegisterIndex::D;
        case 0x03:
        case 0x0B:
            return RegisterIndex::E;
        case 0x04:
        case 0x0C:
            return RegisterIndex::H;
        case 0x05:
        case 0x0D:
            return RegisterIndex::L;
        case 0x07:
        case 0x0F:
            return RegisterIndex::A;
        default:
            return RegisterIndex::F;
    }
}

/*
 * Instruction calling
 */
void Gameboy::call0XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            if (LOGGING) printf("NOOP\n");
            // NO-OP
            break;
        case 0x01:
            loadToRegisterPair(BC);
            break;
        case 0x02:
            loadFromAcc(BC, false);
            break;
        case 0x03:
            incRegisterPair(RegisterPair::BC, 1);            
            break;
        case 0x04:
            incRegister(RegisterIndex::B, 1);
            break;
        case 0x05:
            incRegister(RegisterIndex::B, -1);
            break;
        case 0x06:
            loadToRegister(B);
            break;
        case 0x07:
            RLC(A);
            break;
        case 0x08:
            loadFromStack();
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::BC);
            break;
        case 0x0A:
            loadToAcc(RegisterPair::BC, 0);
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::BC, -1);
            break;
        case 0x0C:
            incRegister(RegisterIndex::C, 1);
            break;
        case 0x0D:
            incRegister(RegisterIndex::C, -1);
            break;
        case 0x0E:
            loadToRegister(C);
            break;
        case 0x0F:
            RRC(A); 
            break;
        default:
            printf("Error: 0 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call1XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            loadToRegisterPair(DE);
            break;
        case 0x02:
            loadFromAcc(DE, false);
            break;
        case 0x03:
            incRegisterPair(RegisterPair::DE, 1);            
            break;
        case 0x04:
            incRegister(RegisterIndex::D, 1);
            break;
        case 0x05:
            incRegister(RegisterIndex::D, -1);
            break;
        case 0x06:
            loadToRegister(D);
            break;
        case 0x07:
            rotateRegisterLeft(A);
            break;
        case 0x08:
            relativeJump(TF, false);
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::DE);
            break;
        case 0x0A:
            loadToAcc(RegisterPair::DE, 0);
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::DE, -1);
            break;
        case 0x0C:
            incRegister(RegisterIndex::E, 1);
            break;
        case 0x0D:
            incRegister(RegisterIndex::E, -1);
            break;
        case 0x0E:
            loadToRegister(E);
            break;
        case 0x0F:
            rotateRegisterRight(A);
            break;
        default:
            printf("Error: 1 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call2XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            relativeJump(ZF, true);
            break;
        case 0x01:
            loadToRegisterPair(HL);
            break;
        case 0x02:
            loadFromAcc(HL, true);
            break;
        case 0x03:
            incRegisterPair(RegisterPair::HL, 1);            
            break;
        case 0x04:
            incRegister(RegisterIndex::H, 1);
            break;
        case 0x05:
            incRegister(RegisterIndex::H, -1);
            break;
        case 0x06:
            loadToRegister(H);
            break;
        case 0x07:
            DAA();
            break;
        case 0x08:
            relativeJump(ZF, false);
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::HL);
            break;
        case 0x0A:
            loadToAcc(RegisterPair::HL, 1);
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::HL, -1);
            break;
        case 0x0C:
            incRegister(RegisterIndex::L, 1);
            break;
        case 0x0D:
            incRegister(RegisterIndex::L, -1);
            break;
        case 0x0E:
            loadToRegister(L);
            break;
        case 0x0F:
            r.registers[A] = ~r.registers[A];
            r.subtract = true;
            r.halfCarry = true;
            r.modifiedFlags = true;
            break;
        default:
            printf("Error: 2 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call3XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            relativeJump(CF, true);
            break;
        case 0x01:
            loadToRegisterPair(RegisterPair::SP);
            break;
        case 0x02:
            loadFromAcc(HL, false);
            break;
        case 0x03:
            incRegisterPair(RegisterPair::SP, 1);
            break;
        case 0x04:
            incMemory(1);
            break;
        case 0x05:
            incMemory(-1);
            break;
        case 0x06:
            loadImmediateDataToMemory();
            break;
        case 0x08:
            relativeJump(CF, false);
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::SP);
            break;
        case 0x0A:
            loadToAcc(RegisterPair::HL, -1);
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::SP, -1);
            break;
        case 0x0C:
            incRegister(RegisterIndex::A, 1);
            break;
        case 0x0D:
            incRegister(RegisterIndex::A, -1);
            break;
        case 0x0E:
            loadToRegister(A);
            break;
        case 0x0F:
            r.subtract = false;
            r.halfCarry = false;
            r.carry = !r.carry;
            r.modifiedFlags = true;
            break;
        default:
            printf("Error: 3 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call4X6XInstructions(RegisterIndex target, unsigned char secondHalfByte) {
    RegisterIndex value = byteToIndex(secondHalfByte);
    
    if (value == RegisterIndex::F) {
        loadFromMemory(target);
        return;
    }

    if (secondHalfByte > 0x07) {
        target = (RegisterIndex)((int)target + 1);
    }

    load(target, value);
}

void Gameboy::call7XInstructions(unsigned char secondHalfByte) {
    if (secondHalfByte > 0x07) { 
        RegisterIndex value = byteToIndex(secondHalfByte);
        
        if (value == RegisterIndex::F) {
            loadFromMemory(A);
            return;
        }

        load(A, value);
        return;
    } else if (secondHalfByte == 0x06) { 
        if(LOGGING) printf("HALT\n");
        printf("Haven't implemented HALT");
        exit(1);
        return; // TODO: Implement HALT
    }

    RegisterIndex index = byteToIndex(secondHalfByte);
    loadToMemory(index);
}

void Gameboy::call8XInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    bool carry = (secondHalfByte > 0x07);

    if (index == RegisterIndex::F) {
        addFromMemory(carry);
        return;
    }

    add(index, carry);
}

void Gameboy::call9XInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    bool carry = (secondHalfByte > 0x07);

    if (index == RegisterIndex::F) {
        subtractFromMemory(carry);
        return;
    }

    subtract(index, carry);
}

void Gameboy::callAXInstructions(unsigned char secondHalfByte) { 
    RegisterIndex index = byteToIndex(secondHalfByte);
    if (index == RegisterIndex::F) {
        if (secondHalfByte > 0x07) {
            bitwiseXorFromMemory();
            return;
        }
        bitwiseAndFromMemory();
        return;
    }

    if (secondHalfByte > 0x07) {
        bitwiseXor(index);
        return;
    }
    
    bitwiseAnd(index);
}

void Gameboy::callBXInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    if (index == RegisterIndex::F) {
        if (secondHalfByte > 0x07) {
            compareFromMemory();
            return;
        }
        bitwiseOrFromMemory();
        return;
    }

    if (secondHalfByte > 0x07) {
        compare(index);
        return;
    }
    
    bitwiseOr(index);
}

void Gameboy::callCXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            ret(ZF, true);
            break;
        case 0x01:
            popToRegisterPair(BC);
            break;
        case 0x03:
            jumpNN();
            break;
        case 0x05:
            pushRegisterPair(BC);
            break;
        case 0x06:
            addImmediate(false);
            break;
        case 0x07:
            restart(0x00);
            break;
        case 0x08:
            ret(ZF, false);
            break;
        case 0x09:
            ret(std::nullopt, false);
            break;
        case 0x0B:
            loadCBInstruction();
            break;
        case 0x0D:
            callFunction();
            break;
        case 0x0E:
            addImmediate(true);
            break;
        case 0x0F:
            restart(0x08);
            break;
        default:
            printf("Error: C unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callDXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            ret(CF, true);
            break;
        case 0x01:
            popToRegisterPair(DE);
            break;
        case 0x05:
            pushRegisterPair(DE);
            break;
        case 0x06:
            subtractImmediate(false);
            break;
        case 0x07:
            restart(0x10);
            break;
        case 0x08:
            ret(CF, false);
            break;
        case 0x09:
            // RETI
            ret(std::nullopt, false);
            IME = true;
            break;
        case 0x0E:
            subtractImmediate(true);
            break;
        case 0x0F:
            restart(0x18);
            break;
        default:
            printf("Error: D unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callEXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            loadFromAcc(false);
            break;
        case 0x01:
            popToRegisterPair(HL);
            break;
        case 0x02:
            loadFromAcc(true);
            break;
        case 0x05:
            pushRegisterPair(HL);
            break;
        case 0x06:
            bitwiseAndImmediate();
            break;
        case 0x07:
            restart(0x20);
            break;
        case 0x09:
            // Jump to HL
            PC = r.getHL();
            break;
        case 0x0A:
            loadAccToMemory();
            break;
        case 0x0E:
            bitwiseXorImmediate();
            break;
        case 0x03:
        case 0x04:
        case 0x0B:
        case 0x0C:
        case 0x0D:
            // Undefined
            break;
        case 0x0F:
            restart(0x28);
            break;
        default:
            printf("Error: E unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callFXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            loadToAcc(false);
            break;
        case 0x01:
            popToRegisterPair(AF);
            break;
        case 0x02:
            loadToAcc(true);
            break;
        case 0x03:
            IME = false;
            break;
        case 0x05:
            pushRegisterPair(AF);
            break;
        case 0x07:
            restart(0x30);
            break;
        case 0x09:
            SP = r.getHL();
            break;
        case 0x0A:
            loadMemoryToAcc();
            break;
        case 0x0B:
            IME = true;
            break;
        case 0x0E:
            compareN();
            break;
        case 0x0F:
            restart(0x38);
            break;
        case 0x04:
        case 0x0C:
        case 0x0D:
            // Unused
            break;
        default:
            printf("Error: F unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}
