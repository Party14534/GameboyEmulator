#include "gtest/gtest.h"
#include <cassert>
#include "../../src/Gameboy/gameboy.h"

TEST(Inc16BitTest, HandlesBCPair) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x03; // INC BC

    // Set register values
    short unsigned int val = 100;
    g.r.setBC(val);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.getBC() == (unsigned short int)101);
}

TEST(Inc16BitTest, HandlesSP) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x33; // INC SP

    // Set register values
    g.SP = 100;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.SP == (unsigned short int)101);
}

TEST(Dec16BitTest, HandlesBCPair) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x0B; // DEC BC

    // Set register values
    short unsigned int val = 100;
    g.r.setBC(val);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.getBC() == (unsigned short int)99);
}

TEST(Dec16BitTest, HandlesSP) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x3B; // DEC SP

    // Set register values
    g.SP = 100;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.SP == (unsigned short int)99);
}

TEST(Add16BitTest, HandlesHLBCPair) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x09; // Add BC to HL

    // Set register values
    g.r.setBC(100);
    g.r.setHL(1);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.getHL() == (unsigned short int)101);
    assert(!g.r.carry);
    assert(!g.r.halfCarry);
}

TEST(Add16BitTest, HandlesCarryPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x09; // Add BC to HL

    // Set register values
    g.r.setBC(0x1000);
    g.r.setHL(0xF000);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.getHL() == (unsigned short int)0);
    assert(g.r.carry);
    assert(!g.r.halfCarry);
}

TEST(Add16BitTest, HandlesHalfCarryPath) {
    Gameboy g("", {0,0}, false, true);

    // Set instruction
    g.mem.mem[0x0000] = 0x09; // Add BC to HL

    // Set register values
    g.r.setBC(1);
    g.r.setHL(4095);

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.getHL() == (unsigned short int)4096);
    assert(!g.r.carry);
    assert(g.r.halfCarry);
}
