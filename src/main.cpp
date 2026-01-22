#include "main.h"
#include <chrono>

typedef  std::chrono::milliseconds ms;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> [savestate_path]" << std::endl;
        return 1;
    }

    std::string romPath = argv[1];
    std::string savestatePath = (argc >= 3) ? argv[2] : "";

    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Gameboy Emulator", sf::Style::Default);
    //win.setVerticalSyncEnabled(true);

    Gameboy g(romPath, win.getSize(), false);

    win.setFramerateLimit(g.mem.memType == MBC3 ? 120 : 60);

    g.deserialize(savestatePath);

    const int MCYCLES_PER_FRAME = 70556;

    while (win.isOpen()) {
        handleEvents(win, g, savestatePath);
        
        for (int i = 0; i < MCYCLES_PER_FRAME; i++) {
            //printf("%d\n", i);
            g.FDE();

            // One M Cycle == 4 T Cycles
            for (int j = 0; j < 4; j++) {
                g.ppu.main();
            }

            if (g.ppu.readyToDraw) {
                // Render handling
                handleRendering(win, g);
            }
        }

        /* Render
        if (g.ppu.readyToDraw) {
            handleRendering(win, g);
        }*/
    }
}
