#include "gtest/gtest.h"
#include "../../src/Gameboy/gameboy.h"

TEST(CallFunctionTest, HappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0110] = 0xCD;

    // Memory location
    g.mem[0x0111] = 0xAF;
    g.mem[0x0112] = 0xFB;

    // Set SP
    g.SP = 0x0055;
    g.PC = 0x0110;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.PC == 0xFBAF);
    assert(g.SP == 0x0053);
    assert(g.mem[g.SP] == 0x13); // lsb of PC
    assert(g.mem[g.SP + 1] == 0x01); // msb of pC
}
