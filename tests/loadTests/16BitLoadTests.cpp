#include "gtest/gtest.h"
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
