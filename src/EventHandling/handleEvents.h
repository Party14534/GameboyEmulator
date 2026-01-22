#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <SFML/Graphics.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "SFML/Window/Keyboard.hpp"
#include <cstdio>
#include "imgui.h"
#include "imgui-SFML.h"


#include "../Gameboy/gameboy.h"

void handleEvents(sf::RenderWindow& win, Gameboy& g, std::string& saveStatePath);

#endif
