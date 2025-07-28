#include "gameboy.h"

Gameboy::Gameboy(std::string _romPath) : romPath(_romPath) {
    r.registers = std::vector<unsigned char>(8);
    mem = std::vector<unsigned char>(0xFFFF); // 65535
}

void Gameboy::FDE() {
    unsigned char instruction = fetch();

    decode(instruction);
}

unsigned char Gameboy::fetch() {
    unsigned char instruction = mem[PC];

    PC++;

    return instruction;
}

void Gameboy::decode(unsigned char instruction) {
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
            // NO-OP
            break;
        case 0x01:
            loadToRegisterPair(BC);
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
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::BC);
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
        default:
            printf("Error: unknown opcode\n");
            exit(1);
    }
}

void Gameboy::call1XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            loadToRegisterPair(DE);
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
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::DE);
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
        default:
            printf("Error: unknown opcode\n");
            exit(1);
    }
}

void Gameboy::call2XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            loadToRegisterPair(HL);
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
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::HL);
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
        default:
            printf("Error: unknown opcode\n");
            exit(1);
    }
}

void Gameboy::call3XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            loadToRegisterPair(RegisterPair::SP);
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
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::SP);
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
        default:
            printf("Error: unknown opcode\n");
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
        call4X6XInstructions(RegisterIndex::A, secondHalfByte);
    } else if (secondHalfByte == 0x06) { 
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
        case 0x01:
            popToRegisterPair(BC);
            break;
        case 0x05:
            pushRegisterPair(BC);
            break;
        default:
            printf("Error: C unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callDXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            popToRegisterPair(DE);
            break;
        case 0x05:
            pushRegisterPair(DE);
            break;
        default:
            printf("Error: D unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callEXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            popToRegisterPair(HL);
            break;
        case 0x05:
            pushRegisterPair(HL);
            break;
        default:
            printf("Error: E unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::callFXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x01:
            popToRegisterPair(AF);
            break;
        case 0x05:
            pushRegisterPair(AF);
            break;
        default:
            printf("Error: F unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}
