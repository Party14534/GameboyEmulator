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
    assert(g.r.zero == false);
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
