#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("");
    g.r.registers[RegisterIndex::A] = 0x1;
    g.r.registers[RegisterIndex::B] = 0x1;
    g.r.registers[RegisterIndex::H] = 0xFF;
    g.r.registers[RegisterIndex::L] = 0x01;
    g.r.carry = 0x01;
    g.mem[0] = 0x8E;
    g.mem[0xFF01] = 0x02;
    g.FDE();

    std::cout << g.r.zero << " " << g.r.subtract << " " << g.r.carry << " " <<
        g.r.halfCarry << "\n";
    printf("A: %02x\n", g.r.registers[0]);

    while (win.isOpen()) {
        // Event handling
        handleEvents(win);

        // Render handling
        handleRendering(win);
    }
}
