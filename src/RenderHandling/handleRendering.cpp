#include "handleRendering.h"
#include <cstdio>

void handleRendering(sf::RenderWindow& win, Gameboy& g) {
    if (!g.ppu.readyToDraw) { return; }

    win.clear(sf::Color(230,215,255));

    // Draw
    printf("drawing\n");
    g.ppu.drawToScreen(win);

    win.display();
}
