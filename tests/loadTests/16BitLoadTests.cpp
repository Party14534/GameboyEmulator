#include "gtest/gtest.h"
#include <cassert>
#include "../../src/Gameboy/gameboy.h"

TEST(Load16BitTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x01;
    g.mem[0x0001] = 0x31;
    g.mem[0x0002] = 0xA3;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 0x0003);
    assert(g.r.getBC() == 0xA331);
}

TEST(Load16BitTest, HandlesSPPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x31;
    g.mem[0x0001] = 0x31;
    g.mem[0x0002] = 0xA3;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 0x0003);
    assert(g.SP == 0xA331);
}

TEST(Pop16BitTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xC1;
    g.mem[0x0001] = 0x31;
    g.mem[0x0002] = 0xA3;
    g.SP = 0x0001;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.SP == 0x0003);
    assert(g.r.getBC() == 0xA331);
}

TEST(Pop16BitTest, HandlesAFPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xF1;
    g.mem[0x0001] = 0xA0; // F's value
    g.mem[0x0002] = 0xFE;
    g.SP = 0x0001;
    g.PC = 0x0000;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.SP == 0x0003);
    assert(g.r.getAF() == 0xFEA0);
    assert(g.r.zero == true);
    assert(g.r.subtract == false);
    assert(g.r.halfCarry == true);
    assert(g.r.carry == false);
}
