#include "handleEvents.h"

void handleEvents(sf::RenderWindow& win, Gameboy& g) {
    while(const std::optional e = win.pollEvent()) {
        if (e->is<sf::Event::Closed>()) {
            win.close();
        } 
        else if (e->is<sf::Event::KeyPressed>()) {
            const auto keyPressed = e->getIf<sf::Event::KeyPressed>();

            switch (keyPressed->scancode) {
                case sf::Keyboard::Scancode::Enter:
                    g.mem.startButton = true;
                    break;
                case sf::Keyboard::Scancode::LShift:
                    g.mem.selectButton = true;
                    break;
                case sf::Keyboard::Scancode::Z:
                    g.mem.aButton = true;
                    break;
                case sf::Keyboard::Scancode::X:
                    g.mem.bButton = true;
                    break;
                default:
                    break;
            }
        }
        else if (e->is<sf::Event::KeyReleased>()) {
            const auto keyReleased = e->getIf<sf::Event::KeyReleased>();

            switch (keyReleased->scancode) {
                case sf::Keyboard::Scancode::Enter:
                    g.mem.startButton = false;
                    break;
                case sf::Keyboard::Scancode::LShift:
                    g.mem.selectButton = false;
                    break;
                case sf::Keyboard::Scancode::Z:
                    g.mem.aButton = false;
                    break;
                case sf::Keyboard::Scancode::X:
                    g.mem.bButton = false;
                    break;
                default:
                    break;
            }
        }
    }
}
