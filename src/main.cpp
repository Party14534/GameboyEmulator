#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("");
    
    /*
    for(int i = 0; i < 457; i++) {
        g.FDE();
        //printf("%d, %d\n", i, g.PC);
    }*/

    unsigned int frameCount = 0;

    while (win.isOpen()) {
        g.FDE();
        g.ppu.main();
        frameCount++;
        
        if (frameCount & 1) continue;

        // Event handling
        handleEvents(win);


        // Render handling
        handleRendering(win, g);
    }
}
