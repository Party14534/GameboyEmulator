#include "gtest/gtest.h"
#include <cassert>
#include "../../src/Gameboy/gameboy.h"

TEST(CBBitTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x40; // bit operation

    g.r.registers[RegisterIndex::B] = 0b10000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesZeroPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x40; // bit operation

    g.r.registers[RegisterIndex::B] = 0b00000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesOddPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x48; // bit operation

    g.r.registers[RegisterIndex::B] = 0b01000000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBBitTest, HandlesHLPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x76; // bit operation

    g.r.setHL(0xFF0F);
    g.mem[0xFF0F] = 0b00000010;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
}

TEST(CBRLTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x10; // bit operation

    g.r.registers[B] = 0b10001111;
    g.r.carry = false;

    // Run Code
    g.FDE();

    // Assert value is correct
    printf("%04x\n", g.r.registers[F]);
    assert(g.r.registers[B] == 0b00011110);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRRTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x18; // bit operation

    g.r.registers[B] = 0b10001111;
    g.r.carry = true;

    // Run Code
    g.FDE();

    // Assert value is correct
    printf("%04x\n", g.r.registers[F]);
    assert(g.r.registers[B] == 0b11000111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRLCTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x00; // bit operation

    g.r.registers[B] = 0b10001111;

    // Run Code
    g.FDE();

    // Assert value is correct
    printf("%04x\n", g.r.registers[F]);
    assert(g.r.registers[B] == 0b00011111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry);
    assert(g.r.registers[F] == 0x10);
}

TEST(CBRRCTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xCB; // CB prefix
    g.mem[0x0001] = 0x08; // bit operation

    g.r.registers[B] = 0b10001110;

    // Run Code
    g.FDE();

    // Assert value is correct
    printf("%04x\n", g.r.registers[F]);
    assert(g.r.registers[B] == 0b01000111);
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == false);
    assert(g.r.registers[F] == 0x00);
}
