#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("", win.getSize(), true);

    unsigned int frameCount = 0;

    while (win.isOpen()) {
        //printf("PC:%d\n", g.mem.read(0xFF50));
        if (LOGGING) printf("LY 0xFF44: %d 0x%02x\n", g.mem.read(0xFF44), g.mem.read(0xFF44));

        for (int i = 0; i < 3; i++) {
            g.FDE();
            g.ppu.main();
        }

        frameCount++;
        if (frameCount & 1) continue;

        // Event handling
        handleEvents(win);


        // Render handling
        handleRendering(win, g);
    }
}
