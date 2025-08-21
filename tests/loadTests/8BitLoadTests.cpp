#include "gtest/gtest.h"
#include "../../src/Gameboy/gameboy.h"

TEST(LoadTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x41;

    // Set register values
    g.r.registers[RegisterIndex::B] = 0x00;
    g.r.registers[RegisterIndex::C] = 0x10;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::B] == 0x10);
}

TEST(LoadTest, IncrementTarget) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x48;

    // Set register values
    g.r.registers[RegisterIndex::B] = 0x10;
    g.r.registers[RegisterIndex::C] = 0x00;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::C] == 0x10);
}

TEST(LoadTest, LoadFromMemory) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x46;

    // Set register values
    g.r.registers[RegisterIndex::B] = 0x10;
    g.r.registers[RegisterIndex::H] = 0xFE;
    g.r.registers[RegisterIndex::L] = 0x01;

    g.mem[0xFE01] = 0x10;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[RegisterIndex::B] == 0x10);
}

TEST(LoadToMemoryTest, HandlesHappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x70;

    // Set register values
    g.r.registers[RegisterIndex::B] = 0x10;
    g.r.registers[RegisterIndex::H] = 0xFE;
    g.r.registers[RegisterIndex::L] = 0x01;
    g.mem[0xFE01] = 0x00;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.mem[0xFE01] == 0x10);
}

TEST(LoadToMemoryTest, Halt) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0x76;

    // Set register values
    g.r.registers[RegisterIndex::B] = 0x10;
    g.r.registers[RegisterIndex::H] = 0xFE;
    g.r.registers[RegisterIndex::L] = 0x01;
    g.mem[0xFE01] = 0x00;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.mem[0xFE01] == 0x00);
}

TEST(LoadAccToMemoryTest, HappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xEA;
    g.mem[0x0001] = 0x10;
    g.mem[0x0002] = 0xFF;

    // Set register values
    g.r.registers[A] = 0xBB;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.mem[0xFF10] == 0xBB);
}

TEST(LoadMemoryToAccTest, HappyPath) {
    Gameboy g("");

    // Set instruction
    g.mem[0x0000] = 0xFA;
    g.mem[0x0001] = 0x10;
    g.mem[0x0002] = 0xFF;

    // Set register values
    g.mem[0xFF10] = 0xBB;

    // Run Code
    g.FDE();

    // Assert value is correct
    assert(g.r.registers[A] == 0xBB);
}
