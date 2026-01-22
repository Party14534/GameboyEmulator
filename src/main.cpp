#include "main.h"

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> [savestate_path]" << std::endl;
        return 1;
    }

    std::string romPath = argv[1];
    std::string savestatePath = (argc >= 3) ? argv[2] : "";

    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Gameboy Emulator", sf::Style::Default);
    
    Gameboy g(romPath, win.getSize(), false);

    g.deserialize(savestatePath);

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double>;
    const double MCYCLE_TIME = 1.0 / 1048576.0;  // Time per M-cycle in seconds

    auto lastTime = Clock::now();
    double cycleAccumulator = 0.0;

    while (win.isOpen()) {
        // Calculate delta time
        auto currentTime = Clock::now();
        Duration deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Convert delta time to M-cycles
        cycleAccumulator += deltaTime.count() / MCYCLE_TIME;

        // Event handling
        handleEvents(win, g, savestatePath);

        /*
        for (int cycles = 0; cycles < 70224; ) {
            int before = g.cycles;
            g.FDE();
            int executed = g.cycles - before;
            
            // Tick PPU for each T-cycle
            for (int i = 0; i < executed; i++) {
                g.ppu.main();
            }
            
            cycles += executed;
        }*/

        for (int i = 0; i < 15; i++) {
            g.FDE();

            // One M Cycle == 4 T Cycles
            for (int j = 0; j < 4; j++) {
                g.ppu.main();
            }
        }

        // Render handling
        handleRendering(win, g);
    }
}
