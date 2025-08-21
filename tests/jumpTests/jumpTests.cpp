#include "gtest/gtest.h"
#include "../../src/Gameboy/gameboy.h"

TEST(JumpNZTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x20;
    g.mem[0x0001] = 20;

    g.r.zero = false;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 22);
}

TEST(JumpNCTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.PC = 18;
    g.mem[18] = 0x30;
    g.mem[19] = -20;

    g.r.carry = false;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 0);
}

TEST(JumpZTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x28;
    g.mem[0x0001] = 20;

    g.r.zero = true;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 22);
}

TEST(JumpCTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.PC = 18;
    g.mem[18] = 0x38;
    g.mem[19] = -20;

    g.r.carry = false;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 20);
}
