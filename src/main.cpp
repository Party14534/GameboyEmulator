#include "main.h"

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> [savestate_path]" << std::endl;
        return 1;
    }

    std::string romPath = argv[1];
    std::string savestatePath = (argc >= 3) ? argv[2] : "";

    unsigned int frameRate = 60;
    /*if (argc >= 4) {
        frameRate = std::atoi(argv[3]);
        frameRate = frameRate == 0 ? 60 : frameRate;
    }*/

    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Gameboy Emulator", sf::Style::Default);
    //win.setVerticalSyncEnabled(true);
    
    // Initialize ImGui-SFML
    int init = ImGui::SFML::Init(win);

    Gameboy g(romPath, win.getSize(), true);
    win.setVerticalSyncEnabled(true);

    win.setFramerateLimit(frameRate);
    //printf("Memtype: %d\n", g.mem.memType);

    g.deserialize(savestatePath);

    const int MCYCLES_PER_FRAME = 70556 / 4;

    // ImGui state
    int currentFrameRate = frameRate;
    sf::Clock deltaClock;

    while (win.isOpen()) {
        // Update ImGui
        ImGui::SFML::Update(win, deltaClock.restart());
        
        handleEvents(win, g, savestatePath);

        // ImGui Window for settings
        ImGui::Begin("Emulator Settings");

        // LOAD STATE BUTTON
        if (ImGui::Button("Load State")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("LoadStateDlg", 
                "Choose Save State", ".sav,.state", config);
        }

        ImGui::SameLine();

        // SAVE STATE BUTTON
        if (ImGui::Button("Save State")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_ConfirmOverwrite; // Ask before overwriting
            ImGuiFileDialog::Instance()->OpenDialog("SaveStateDlg", 
                "Save State As", ".sav,.state", config);
        }

        // Handle LOAD dialog
        if (ImGuiFileDialog::Instance()->Display("LoadStateDlg")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                
                // Load your save state
                g.deserialize(filePath);
                printf("Loaded state from: %s\n", filePath.c_str());
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // Handle SAVE dialog
        if (ImGuiFileDialog::Instance()->Display("SaveStateDlg")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                
                // Make sure it has the right extension
                if (filePath.find(".sav") == std::string::npos && 
                    filePath.find(".state") == std::string::npos) {
                    filePath += ".sav";
                }
                
                // Save your state
                std::ofstream os(filePath, std::ios::binary);
                if (os.is_open()) {
                    cereal::BinaryOutputArchive archive(os);
                    archive(g);
                    os.close();
                    printf("Saved state to: %s\n", filePath.c_str());
                } else {
                    printf("Failed to save state to: %s\n", filePath.c_str());
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::SliderInt("Frame Rate", &currentFrameRate, 1, 240)) {
            currentFrameRate = currentFrameRate & 1 ? currentFrameRate ^ 1 : currentFrameRate; 
            win.setFramerateLimit(currentFrameRate);
        }
        
        ImGui::Text("FPS: %d", currentFrameRate);
        
        ImGui::End();
        
        for (int i = 0; i < MCYCLES_PER_FRAME; i++) {
            g.FDE();

            // One M Cycle == 4 T Cycles
            for (int j = 0; j < 4; j++) {
                g.ppu.main();
            }

        }

        win.clear(sf::Color::Black);

        g.ppu.drawToScreen(win);

        ImGui::SFML::Render(win);

        win.display();
    }

    // Cleanup ImGui
    ImGui::SFML::Shutdown();
}
