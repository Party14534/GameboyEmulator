#include "handleEvents.h"

void handleEvents(sf::RenderWindow& win, Gameboy& g) {
    while(const std::optional e = win.pollEvent()) {
        ImGui::SFML::ProcessEvent(win, *e);

        if (e->is<sf::Event::Closed>()) {
            win.close();
        } 
    }
}
