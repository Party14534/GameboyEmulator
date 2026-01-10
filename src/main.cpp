#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("", win.getSize(), false);

    //unsigned int frameCount = 0;

    while (win.isOpen()) {
        //printf("PC:%d\n", g.mem.read(0xFF50));
        if (LOGGING) printf("LY 0xFF44: %d 0x%02x\n", g.mem.read(0xFF44), g.mem.read(0xFF44));

        // Event handling
        handleEvents(win, g);

        for (int i = 0; i < 5; i++) {
            g.FDE();

            // One M Cycle == 4 T Cycles
            for (int j = 0; j < 4; j++) {
                g.ppu.main();
            }
        }

        //frameCount++;
        //if (frameCount & 1 && !g.ppu.readyToDraw) continue;

        // Render handling
        handleRendering(win, g);
    }
}
