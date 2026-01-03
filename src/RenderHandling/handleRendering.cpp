#include "handleRendering.h"

void handleRendering(sf::RenderWindow& win, Gameboy& g) {
    if (!g.ppu.readyToDraw) { return; }

    win.clear(sf::Color::Black);

    // Draw
    g.ppu.drawToScreen(win);
    //printf("0x%02x\n", g.mem.mem[LCDC_ADDR]);

    win.display();
}
