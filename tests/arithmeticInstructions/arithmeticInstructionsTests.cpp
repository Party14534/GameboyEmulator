#include "gtest/gtest.h"
#include <cassert>
#include "../../src/Gameboy/gameboy.h"

TEST(AdditionTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x80;

    // Set register values
    g.r.registers[RegisterIndex::A] = 0x0010;
    g.r.registers[RegisterIndex::B] = 0x0010;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::A] == 0x0020);

    // Assert flags set correctly
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == false);
}

TEST(AdditionTest, HandlesHalfCarryPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x80;

    // Set register values
    g.r.registers[RegisterIndex::A] = (unsigned char)62;
    g.r.registers[RegisterIndex::B] = (unsigned char)34;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::A] == 96);

    // Assert flags set correctly
    assert(g.r.zero == false);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
    assert(g.r.carry == false);
}

TEST(AdditionTest, HandlesCarryPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x80;

    // Set register values
    g.r.registers[RegisterIndex::A] = 0xF0;
    g.r.registers[RegisterIndex::B] = 0x10;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::A] == 0);

    // Assert flags set correctly
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == false);
    assert(g.r.carry == true);
}
