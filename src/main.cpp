#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g("");
    
    for(int i = 0; i < 257; i++) {
        g.FDE();
        printf("%d, %d\n", i, g.PC);
    }

    return 0;
    while (win.isOpen()) {
        // Event handling
        handleEvents(win);
                
        // Render handling
        handleRendering(win);
    }
}
