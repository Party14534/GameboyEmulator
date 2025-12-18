#include "gtest/gtest.h"
#include <cassert>
#include "../../src/Gameboy/gameboy.h"

TEST(CBBitTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x40; // bit operation

    g.r.registers[RegisterIndex::B] = 0b10000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesZeroPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x40; // bit operation

    g.r.registers[RegisterIndex::B] = 0b00000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesOddPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x48; // bit operation

    g.r.registers[RegisterIndex::B] = 0b01000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesHLPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x76; // bit operation

    g.r.setHL(0xFF0F);
    g.mem.write(0xFF0F, 0b00000010);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBRLTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x10; // bit operation

    g.r.registers[B] = 0b10001111;
    g.r.carry = false;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b00011110);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRRTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x18; // bit operation

    g.r.registers[B] = 0b10001111;
    g.r.carry = true;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b11000111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRLCTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x00; // bit operation

    g.r.registers[B] = 0b10001111;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b00011111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRRCTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x08; // bit operation

    g.r.registers[B] = 0b10001110;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b01000111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == false);
    assert(g.r.registers[F] == 0x00);
}

TEST(CBSwapTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x36; // bit operation
    g.mem.mem[0xFF44] = 0b10010110; // bit operation

    g.r.registers[H] = 0xFF;
    g.r.registers[L] = 0x44;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.mem.read(0xFF44) == 0b01101001);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == false);
    assert(g.r.registers[F] == 0x00);
}

TEST(CBSwapTest, HandlesZeroPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x30; // bit operation

    g.r.registers[B] = 0b00000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b00000000);
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == false);
    assert(g.r.registers[F] == 0x80);
}

TEST(CBSrlTest, HandlesHappyPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x3E; // bit operation
    g.mem.mem[0xFF44] = 0b10000001; // bit operation

    g.r.registers[H] = 0xFF;
    g.r.registers[L] = 0x44;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.mem.read(0xFF44) == 0b01000000);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == true);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBSrlTest, HandlesZeroPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0xCB; // CB prefix
    g.mem.mem[0x0001] = 0x38; // bit operation
    g.r.registers[B] = 0b00000001; // bit operation

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[B] == 0b00000000);
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == true);
    assert(g.r.registers[F] == 0x90);
}
