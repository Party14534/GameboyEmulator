#include "main.h"

int main() {
    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Template", sf::Style::Default);
    
    Gameboy g;

    while (win.isOpen()) {
        // Event handling
        handleEvents(win);

        // Render handling
        handleRendering(win);
    }
}
