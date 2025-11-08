#include "handleRendering.h"

void handleRendering(sf::RenderWindow& win, Gameboy& g) {
    if (!g.ppu.readyToDraw) { return; }

    win.clear(sf::Color::Black);

    // Draw
    g.ppu.drawToScreen(win);

    win.display();
}
