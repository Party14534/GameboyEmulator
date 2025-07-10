#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("");
    g.r.registers[RegisterIndex::A] = 0x2;
    g.r.registers[RegisterIndex::B] = 0xF;
    g.r.registers[RegisterIndex::H] = 0xFF;
    g.r.registers[RegisterIndex::L] = 0x01;
    g.r.carry = 0x01;
    g.mem[0] = 0x9E;
    g.mem[0xFF01] = 0x02;
    g.FDE();

    std::cout << g.r.zero << "zero " << g.r.subtract << "sub " << 
        g.r.carry << "carry " << g.r.halfCarry << "half\n";
    printf("A: %02x\n", g.r.registers[0]);
    
    return 0;
    while (win.isOpen()) {
        // Event handling
        handleEvents(win);
        
        // Render handling
        handleRendering(win);
    }
}
