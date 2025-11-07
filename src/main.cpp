#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("");
    //g.PC = 0x100;
    
    /*
    for(int i = 0; i < 457; i++) {
        g.FDE();
        //printf("%d, %d\n", i, g.PC);
    }*/

    unsigned int frameCount = 0;

    while (win.isOpen()) {
        g.FDE();
        if (LOGGING) printf("LY 0xFF44: %d 0x%02x\n", g.mem[0xFF44], g.mem[0xFF44]);
        g.ppu.main();
        frameCount++;
        //if (frameCount & 1) continue;

        // Event handling
        handleEvents(win);


        // Render handling
        handleRendering(win, g);
    }
}
