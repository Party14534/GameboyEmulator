#ifndef RENDER_HANDLING_H
#define RENDER_HANDLING_H

#include <SFML/Graphics.hpp>

#include "../Gameboy/gameboy.h"
#include "imgui.h"
#include "imgui-SFML.h"

void handleRendering(sf::RenderWindow& win, Gameboy& g);

#endif
