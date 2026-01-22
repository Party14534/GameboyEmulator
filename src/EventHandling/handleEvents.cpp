#include "handleEvents.h"
#include "SFML/Window/Keyboard.hpp"
#include <cstdio>

void handleEvents(sf::RenderWindow& win, Gameboy& g, std::string& saveStatePath) {
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
                case sf::Keyboard::Scancode::RShift:
                    g.mem.selectButton = true;
                    break;
                case sf::Keyboard::Scancode::X:
                    g.mem.aButton = true;
                    break;
                case sf::Keyboard::Scancode::Z:
                    g.mem.bButton = true;
                    break;
                case sf::Keyboard::Scancode::Up:
                    g.mem.upButton = true;
                    break;
                case sf::Keyboard::Scancode::Down:
                    g.mem.downButton = true;
                    break;
                case sf::Keyboard::Scancode::Left:
                    g.mem.leftButton = true;
                    break;
                case sf::Keyboard::Scancode::Right:
                    g.mem.rightButton = true;
                    break;
                default:
                    break;
            }
        }
        else if (e->is<sf::Event::KeyReleased>()) {
            const auto keyReleased = e->getIf<sf::Event::KeyReleased>();

            // Check for Ctrl+S to save state
            if (keyReleased->scancode == sf::Keyboard::Scancode::S && 
                (keyReleased->control || keyReleased->system)) {
                if (saveStatePath != "") {
                    std::ofstream os(saveStatePath, std::ios::binary);
                    cereal::BinaryOutputArchive archive(os);
                    archive(g);
                    std::cout << "Game saved to: " << saveStatePath << std::endl;
                } else {
                    printf("No save state path provided\n");
                }

                continue;
            }

            switch (keyReleased->scancode) {
                case sf::Keyboard::Scancode::Enter:
                    g.mem.startButton = false;
                    break;
                case sf::Keyboard::Scancode::RShift:
                    g.mem.selectButton = false;
                    break;
                case sf::Keyboard::Scancode::X:
                    g.mem.aButton = false;
                    break;
                case sf::Keyboard::Scancode::Z:
                    g.mem.bButton = false;
                    break;
                case sf::Keyboard::Scancode::Up:
                    g.mem.upButton = false;
                    break;
                case sf::Keyboard::Scancode::Down:
                    g.mem.downButton = false;
                    break;
                case sf::Keyboard::Scancode::Left:
                    g.mem.leftButton = false;
                    break;
                case sf::Keyboard::Scancode::Right:
                    g.mem.rightButton = false;
                    break;
                default:
                    break;
            }
        }
    }
}
