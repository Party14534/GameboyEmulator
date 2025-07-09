#include "handleEvents.h"

void handleEvents(sf::RenderWindow& win) {
    while(const std::optional e = win.pollEvent()) {
        if (e->is<sf::Event::Closed>()) {
            win.close();
        }
    }
}
