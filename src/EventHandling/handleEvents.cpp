#include "handleEvents.h"

void handleEvents(sf::RenderWindow& win, Gameboy& g) {
    while(const std::optional e = win.pollEvent()) {
        ImGui::SFML::ProcessEvent(win, *e);

        if (e->is<sf::Event::Closed>()) {
            win.close();
        } else if (e->is<sf::Event::FocusGained>()) {
            g.mem.windowFocused = true;
        } else if (e->is<sf::Event::FocusLost>()) {
            g.mem.windowFocused = false;
        }
    }
}
