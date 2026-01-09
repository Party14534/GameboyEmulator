#include "gameboy.h"

Gameboy::Gameboy(std::string _romPath, sf::Vector2u winSize, bool bootRom, bool _testing) : 
    testing(_testing),
    romPath(_romPath),
    mem(PC), // 65535
    ppu(mem, winSize)
{
    r.registers = std::vector<unsigned char>(8);

    // Used for unit tests
    if (testing) { 
        mem.mem[0xFF50] = 1;
        return; 
    }
    
    writeRom();
    writeBootRom();

    if (!bootRom) { 
        mem.write(0xFF50, 1);

        // Correct state after boot rom
        r.registers[A] = 0x01;
        r.registers[F] = 0xB0;
        r.registers[B] = 0x00;
        r.registers[C] = 0x13;
        r.registers[D] = 0x00;
        r.registers[E] = 0xD8;
        r.registers[H] = 0x01;
        r.registers[L] = 0x4D;
        SP = 0xFFFE;
        PC = 0x100;
        r.setFlags();

        
        // ✓ Initialize I/O registers (post-boot state)
        mem.write(0xFF40, 0x91);  // LCDC - LCD on, BG on
        mem.write(0xFF47, 0xFC);  // BGP - palette
        mem.write(0xFF48, 0xFF);  // OBP0
        mem.write(0xFF49, 0xFF);  // OBP1
        mem.write(0xFF0F, 0x00);  // IF - no interrupts
        mem.write(0xFFFF, 0x00);  // IE - interrupts disabled
    }

    IE = &mem.mem[0xFFFF];

    paletteOne = std::vector<sf::Color>{
        sf::Color(155, 188, 15),
        sf::Color(139, 172, 15),
        sf::Color(48, 98, 48),
        sf::Color(15, 56, 15)
    };

    if (DOCTOR_LOGGING) {
        printf("A:%02x F:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x SP:%04x PC:%04x PCMEM:%02x,%02x,%02x,%02x\n",
                r.registers[A], 
                r.registers[F],
                r.registers[B],
                r.registers[C],
                r.registers[D],
                r.registers[E],
                r.registers[H],
                r.registers[L],
                SP, PC,
                mem.read(PC), mem.read(PC+1), mem.read(PC+2),
                mem.read(PC+3));
    }

    DIV = &mem.mem[DIV_ADDR];
    TIMA = &mem.mem[TIMA_ADDR];

    cycles = 0;
}

void Gameboy::writeBootRom() {
    std::ifstream file ("../src/res/dmg_boot.bin", std::ios::binary);

    // Read entire file into vector
    mem.bootRomMem.assign(std::istreambuf_iterator<char>(file),
               std::istreambuf_iterator<char>());

    // Write cartridge header
    if (!WRITEHEADER) return;

    unsigned char header[] = {
        0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e
    };
    
    for (int i = 0x104; i <= 0x133; i++) {
        mem.write(i, header[i - 0x104]);
    }
}

void Gameboy::writeRom() {
    //std::ifstream file ("../tests/dmg-acid2.gb", std::ios::binary);
    //std::ifstream file ("../tests/m3_bgp_change_sprites.gb", std::ios::binary);
    //std::ifstream file ("../tests/11.gb", std::ios::binary);
    std::ifstream file ("../roms/tetris.gb", std::ios::binary);
    if (!file.good()) { 
        printf("file doesn't exist\n");
        exit(1);
    }

    std::stringstream buff;
    buff << file.rdbuf();

    int i = 0x000;
    for (unsigned char byte : buff.str()) {
        mem.write(i, byte);
        i++;
    }
}

void Gameboy::FDE() {
    timer();

    cycles--;
    if (cycles > 0 && !testing) { return; }

    if (halted) {
        // Check if we should wake up
        unsigned char* IF = &mem.mem[IF_ADDR];
        if ((*IF & *IE & 0x1F) != 0) {
            // Interrupt pending - wake up
            halted = false;

            // HALT bug: if IME=0, don't increment PC (next instruction executes twice)
            if (!IME) {
                //PC--;  // HALT bug
            }
        } else {
            // Still halted - don't execute instructions
            cycles = 1;  // Wait 1 M-cycle
            return;
        }
    }

    static int instrCount = 0;

    instrCount++;
    if (instrCount % 1000 == 0) {
        //printf("Executed %d instructions, PC=0x%04x, cycles=%d\n", instrCount, PC, cycles);
    }

    unsigned char instruction = fetch();

    decode(instruction);

    if (r.modifiedFlags) { r.setF(); }
    r.modifiedFlags = false;

    if(LOGFLAGS && LOGGING) printf("zero: %d, sub: %d, half: %d, carry: %d\n",
            r.zero, r.subtract, r.halfCarry, r.carry);

    if (DOCTOR_LOGGING) {
        printf("A:%02x F:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x SP:%04x PC:%04x PCMEM:%02x,%02x,%02x,%02x\n",
                r.registers[A], 
                r.registers[F],
                r.registers[B],
                r.registers[C],
                r.registers[D],
                r.registers[E],
                r.registers[H],
                r.registers[L],
                SP, PC,
                mem.read(PC), mem.read(PC+1), mem.read(PC+2),
                mem.read(PC+3));
    }
    
    if (!IME) { return; } 
    
    unsigned char* IF = &mem.mem[IF_ADDR];

    // Interrupt
    if ((*IF & *IE & 0x1F) != 0) {
        IME = 0;

        SP--;
        mem.write(SP, (PC >> 8) & 0xFF);
        SP--;
        mem.write(SP, PC & 0xFF);

        unsigned char addr = 0;
        if ((*IF & *IE & 1) != 0) {
            addr = VBLANK_INTERRUPT_VECTOR;
            *IF &= 0xFE;
            //printf("VBLANK\n");
        }
        else if ((*IF & *IE & 2) != 0) {
            addr = LCD_INTERRUPT_VECTOR;
            *IF &= 0xFD;
            //printf("LCD\n");
        }
        else if ((*IF & *IE & 4) != 0) {
            addr = TIMER_INTERRUPT_VECTOR;
            *IF &= 0xFB;
            //printf("TIMER\n");
        }
        else if ((*IF & *IE & 8) != 0) {
            addr = SERIAL_INTERRUPT_VECTOR;
            *IF &= 0xF7;
            //printf("SERIAL\n");
        }
        else if ((*IF & *IE & 0x10) != 0) {
            addr = JOYPAD_INTERRUPT_VECTOR;
            *IF &= 0xEF;
            //printf("JOYPAD\n");
        }

        PC = addr;
        cycles += 20;
        // TODO I think there's other stuff
        // IE handling takes 20 cycles
    }
}

void Gameboy::timer() {
    cyclesSinceLastTima++;

    // Timer work
    *DIV = *DIV + 1; // TODO: Is this how often we should update this

    unsigned char TAC = mem.mem[TAC_ADDR];
    if ((TAC & 4) == 0) { return; }

    unsigned char clockSelect = TAC & 3;
    int timaTimer;
    switch (clockSelect) {
        case 0b00:
            timaTimer = 256;
            break;
        case 0b01:
            timaTimer = 4;
            break;
        case 0b10:
            timaTimer = 16;
            break;
        case 0b11:
            timaTimer = 64;
            break;
        default:
            printf("Unknown clock select 0x%02x\n", TAC);
            exit(1);
            break;
    }

    if (cyclesSinceLastTima < timaTimer) { return; }

    cyclesSinceLastTima = 0;
    
    unsigned char prevTima = *TIMA;
    *TIMA = *TIMA + 1;

    // When TIMA overflows reset to TMA
    if (*TIMA < prevTima) {
        *TIMA = mem.mem[TMA_ADDR];
        mem.mem[IF_ADDR] |= 0x04; // Timer interrupt
    }
}

unsigned char Gameboy::fetch() {
    unsigned char instruction = mem.read(PC);

    PC++;
    // TODO: Find actual problem
    if (PC == 0xFA) { 
        PC = 0xFC; 
    }

    return instruction;
}

void Gameboy::decode(unsigned char instruction) {
    if(LOGGING && instruction != 0x76) printf("---\nINSTRUCTION: %02x | PC: %d 0x%04x\n", instruction, PC - 1, PC - 1);
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
            cycles = 1;
            break;
        case 0x01:
            loadToRegisterPair(BC);
            cycles = 3;
            break;
        case 0x02:
            loadFromAcc(BC, false);
            cycles = 2;
            break;
        case 0x03:
            incRegisterPair(RegisterPair::BC, 1);            
            cycles = 2;
            break;
        case 0x04:
            incRegister(RegisterIndex::B, 1);
            cycles = 1;
            break;
        case 0x05:
            incRegister(RegisterIndex::B, -1);
            cycles = 1;
            break;
        case 0x06:
            loadToRegister(B);
            cycles = 2;
            break;
        case 0x07:
            RLC(A);
            r.zero = false;
            cycles = 1;
            break;
        case 0x08:
            loadFromStack();
            cycles = 5;
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::BC);
            cycles = 2;
            break;
        case 0x0A:
            loadToAcc(RegisterPair::BC, 0);
            cycles = 2;
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::BC, -1);
            cycles = 2;
            break;
        case 0x0C:
            incRegister(RegisterIndex::C, 1);
            cycles = 1;
            break;
        case 0x0D:
            incRegister(RegisterIndex::C, -1);
            cycles = 1;
            break;
        case 0x0E:
            loadToRegister(C);
            cycles = 2;
            break;
        case 0x0F:
            RRC(A); 
            r.zero = false;
            cycles = 1;
            break;
        default:
            printf("Error: 0 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call1XInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            IME = 0;
            // TODO: cancel any scheduled effects of EI if any
            cycles = 1;
            break;
        case 0x01:
            loadToRegisterPair(DE);
            cycles = 3;
            break;
        case 0x02:
            loadFromAcc(DE, false);
            cycles = 2;
            break;
        case 0x03:
            incRegisterPair(RegisterPair::DE, 1);            
            cycles = 2;
            break;
        case 0x04:
            incRegister(RegisterIndex::D, 1);
            cycles = 1;
            break;
        case 0x05:
            incRegister(RegisterIndex::D, -1);
            cycles = 1;
            break;
        case 0x06:
            loadToRegister(D);
            cycles = 2;
            break;
        case 0x07:
            rotateRegisterLeft(A);
            r.zero = false;
            cycles = 1;
            break;
        case 0x08:
            relativeJump(TF, false);
            cycles = 3;
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::DE);
            cycles = 2;
            break;
        case 0x0A:
            loadToAcc(RegisterPair::DE, 0);
            cycles = 2;
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::DE, -1);
            cycles = 2;
            break;
        case 0x0C:
            incRegister(RegisterIndex::E, 1);
            cycles = 1;
            break;
        case 0x0D:
            incRegister(RegisterIndex::E, -1);
            cycles = 1;
            break;
        case 0x0E:
            loadToRegister(E);
            cycles = 2;
            break;
        case 0x0F:
            rotateRegisterRight(A);
            r.zero = 0;
            cycles = 1;
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
            // Cycles set in function
            break;
        case 0x01:
            loadToRegisterPair(HL);
            cycles = 3;
            break;
        case 0x02:
            loadFromAcc(HL, true);
            cycles = 2;
            break;
        case 0x03:
            incRegisterPair(RegisterPair::HL, 1);            
            cycles = 2;
            break;
        case 0x04:
            incRegister(RegisterIndex::H, 1);
            cycles = 1;
            break;
        case 0x05:
            incRegister(RegisterIndex::H, -1);
            cycles = 1;
            break;
        case 0x06:
            loadToRegister(H);
            cycles = 2;
            break;
        case 0x07:
            DAA();
            cycles = 1;
            break;
        case 0x08:
            relativeJump(ZF, false);
            // Cycles set in function
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::HL);
            cycles = 2;
            break;
        case 0x0A:
            loadToAcc(RegisterPair::HL, 1);
            cycles = 2;
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::HL, -1);
            cycles = 2;
            break;
        case 0x0C:
            incRegister(RegisterIndex::L, 1);
            cycles = 1;
            break;
        case 0x0D:
            incRegister(RegisterIndex::L, -1);
            cycles = 1;
            break;
        case 0x0E:
            loadToRegister(L);
            cycles = 2;
            break;
        case 0x0F:
            r.registers[A] = ~r.registers[A];
            r.subtract = true;
            r.halfCarry = true;
            r.modifiedFlags = true;
            cycles = 1;
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
            // Cycles set in function
            break;
        case 0x01:
            loadToRegisterPair(RegisterPair::SP);
            cycles = 3;
            break;
        case 0x02:
            loadFromAcc(HL, false);
            cycles = 2;
            break;
        case 0x03:
            incRegisterPair(RegisterPair::SP, 1);
            cycles = 2;
            break;
        case 0x04:
            incMemory(1);
            cycles = 3;
            break;
        case 0x05:
            incMemory(-1);
            cycles = 3;
            break;
        case 0x06:
            loadImmediateDataToMemory();
            cycles = 3;
            break;
        case 0x07:
            r.subtract = 0;
            r.halfCarry = 0;
            r.carry = 1;
            r.modifiedFlags = true;
            cycles = 1;
            break;
        case 0x08:
            relativeJump(CF, false);
            // Cycles set in function
            break;
        case 0x09:
            addRegisterPairs(RegisterPair::HL, RegisterPair::SP);
            cycles = 2;
            break;
        case 0x0A:
            loadToAcc(RegisterPair::HL, -1);
            cycles = 2;
            break;
        case 0x0B:
            incRegisterPair(RegisterPair::SP, -1);
            cycles = 2;
            break;
        case 0x0C:
            incRegister(RegisterIndex::A, 1);
            cycles = 1;
            break;
        case 0x0D:
            incRegister(RegisterIndex::A, -1);
            cycles = 1;
            break;
        case 0x0E:
            loadToRegister(A);
            cycles = 2;
            break;
        case 0x0F:
            r.subtract = false;
            r.halfCarry = false;
            r.carry = !r.carry;
            r.modifiedFlags = true;
            cycles = 1;
            break;
        default:
            printf("Error: 3 unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}

void Gameboy::call4X6XInstructions(RegisterIndex target, unsigned char secondHalfByte) {
    RegisterIndex value = byteToIndex(secondHalfByte);
    
    if (value == RegisterIndex::F) {
        if (secondHalfByte > 0x07) {
            target = (RegisterIndex)((int)target + 1);
        }

        loadFromMemory(target);
        cycles = 2;
        return;
    }

    if (secondHalfByte > 0x07) {
        target = (RegisterIndex)((int)target + 1);
    }

    load(target, value);
    cycles = 1;
}

void Gameboy::call7XInstructions(unsigned char secondHalfByte) {
    if (secondHalfByte > 0x07) { 
        RegisterIndex value = byteToIndex(secondHalfByte);
        
        if (value == RegisterIndex::F) {
            loadFromMemory(A);
            cycles = 2;
            return;
        }

        load(A, value);
        cycles = 1;
        return;
    } else if (secondHalfByte == 0x06) { 
        //if(LOGGING) printf("HALT\n");
        //PC--;
        //printf("Haven't implemented HALT %02x %d\n", PC, PC);
        //exit(1);
        halted = true;
        cycles = 1;
        return; // TODO: Implement HALT
    }

    RegisterIndex index = byteToIndex(secondHalfByte);
    loadToMemory(index);
    cycles = 2;
}

void Gameboy::call8XInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    bool carry = (secondHalfByte > 0x07);

    if (index == RegisterIndex::F) {
        addFromMemory(carry);
        cycles = 2;
        return;
    }

    add(index, carry);
    cycles = 1;
}

void Gameboy::call9XInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    bool carry = (secondHalfByte > 0x07);

    if (index == RegisterIndex::F) {
        subtractFromMemory(carry);
        cycles = 2;
        return;
    }

    subtract(index, carry);
    cycles = 1;
}

void Gameboy::callAXInstructions(unsigned char secondHalfByte) { 
    RegisterIndex index = byteToIndex(secondHalfByte);
    if (index == RegisterIndex::F) {
        cycles = 2;

        if (secondHalfByte > 0x07) {
            bitwiseXorFromMemory();
            return;
        }
        bitwiseAndFromMemory();
        return;
    }

    cycles = 1;

    if (secondHalfByte > 0x07) {
        bitwiseXor(index);
        return;
    }
    
    bitwiseAnd(index);
}

void Gameboy::callBXInstructions(unsigned char secondHalfByte) {
    RegisterIndex index = byteToIndex(secondHalfByte);
    if (index == RegisterIndex::F) {
        cycles = 2;

        if (secondHalfByte > 0x07) {
            compareFromMemory();
            return;
        }
        bitwiseOrFromMemory();
        return;
    }

    cycles = 1;

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
            // Cycles set in function
            break;
        case 0x01:
            popToRegisterPair(BC);
            cycles = 3;
            break;
        case 0x02:
            jumpNN(Flag::ZF, true);
            // Cycles set in function
            break;
        case 0x03:
            jumpNN(std::nullopt, false);
            // Cycles set in function
            break;
        case 0x04:
            callNN(Flag::ZF, true);
            // Cycles set in function
            break;
        case 0x05:
            pushRegisterPair(BC);
            cycles = 4;
            break;
        case 0x06:
            addImmediate(false);
            cycles = 2;
            break;
        case 0x07:
            restart(0x00);
            cycles = 4;
            break;
        case 0x08:
            ret(ZF, false);
            // Cycles set in function
            break;
        case 0x09:
            ret(std::nullopt, false);
            // Cycles set in function
            break;
        case 0x0A:
            jumpNN(Flag::ZF, false);
            // Cycles set in function
            break;
        case 0x0B:
            loadCBInstruction();
            // TODO
            break;
        case 0x0C:
            callNN(Flag::ZF, false);
            // Cycles set in function
            break;
        case 0x0D:
            callFunction();
            cycles = 6;
            break;
        case 0x0E:
            addImmediate(true);
            cycles = 2;
            break;
        case 0x0F:
            restart(0x08);
            cycles = 4;
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
            // Cycles set in function
            break;
        case 0x01:
            popToRegisterPair(DE);
            cycles = 3;
            break;
        case 0x02:
            jumpNN(Flag::CF, true);
            // Cycles set in function
            break;
        case 0x04:
            callNN(Flag::CF, true);
            // Cycles set in function
            break;
        case 0x05:
            pushRegisterPair(DE);
            cycles = 4;
            break;
        case 0x06:
            subtractImmediate(false);
            cycles = 2;
            break;
        case 0x07:
            restart(0x10);
            cycles = 4;
            break;
        case 0x08:
            ret(CF, false);
            // Cycles set in function
            break;
        case 0x09:
            // RETI
            ret(std::nullopt, false);
            IME = true;
            cycles = 4;
            break;
        case 0x0A:
            jumpNN(Flag::CF, false);
            // Cycles set in function
            break;
        case 0x0C:
            callNN(Flag::CF, false);
            // Cycles set in function
            break;
        case 0x0E:
            subtractImmediate(true);
            cycles = 2;
            break;
        case 0x0F:
            restart(0x18);
            cycles = 4;
            break;
        default:
            printf("Error: D unknown opcode %04x PC %04x:%d\n", secondHalfByte, PC, PC);
            exit(1);
    }
}

void Gameboy::callEXInstructions(unsigned char secondHalfByte) {
    switch (secondHalfByte) {
        case 0x00:
            loadFromAcc(false);
            cycles = 3;
            break;
        case 0x01:
            popToRegisterPair(HL);
            cycles = 3;
            break;
        case 0x02:
            loadFromAcc(true);
            cycles = 2;
            break;
        case 0x05:
            pushRegisterPair(HL);
            cycles = 4;
            break;
        case 0x06:
            bitwiseAndImmediate();
            cycles = 2;
            break;
        case 0x07:
            restart(0x20);
            cycles = 4;
            break;
        case 0x08:
            addEToSP();
            cycles = 4;
            break;
        case 0x09:
            // Jump to HL
            PC = r.getHL();
            cycles = 1;
            break;
        case 0x0A:
            loadAccToMemory();
            cycles = 4;
            break;
        case 0x0E:
            bitwiseXorImmediate();
            cycles = 2;
            break;
        case 0x03:
        case 0x04:
        case 0x0B:
        case 0x0C:
        case 0x0D:
            // Undefined
            printf("Undefined E opcode executed\n");
            break;
        case 0x0F:
            restart(0x28);
            cycles = 4;
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
            cycles = 3;
            break;
        case 0x01:
            popToRegisterPair(AF);
            cycles = 3;
            break;
        case 0x02:
            loadToAcc(true);
            cycles = 2;
            break;
        case 0x03:
            IME = false;
            cycles = 1;
            break;
        case 0x05:
            pushRegisterPair(AF);
            cycles = 4;
            break;
        case 0x06:
            bitwiseOrImmediate();
            cycles = 2;
            break;
        case 0x07:
            restart(0x30);
            cycles = 4;
            break;
        case 0x08:
            addImmediateAndSPToHL();
            cycles = 3;
            break;
        case 0x09:
            SP = r.getHL();
            cycles = 2;
            break;
        case 0x0A:
            loadMemoryToAcc();
            cycles = 4;
            break;
        case 0x0B:
            IME = true;
            cycles = 1;
            break;
        case 0x0E:
            compareN();
            cycles = 2;
            break;
        case 0x0F:
            restart(0x38);
            cycles = 4;
            break;
        case 0x04:
        case 0x0C:
        case 0x0D:
            // Unused
            printf("Undefined F opcode executed\n");
            break;
        default:
            printf("Error: F unknown opcode %04x\n", secondHalfByte);
            exit(1);
    }
}
