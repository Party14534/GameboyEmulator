#include "main.h"
#include "Internal/fpsHandling.h"

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> [boot_rom_path]" << std::endl;
        return 1;
    }

    std::string romPath = argv[1];
    std::string bootRomPath = (argc >= 3) ? argv[2] : "";

    /*if (argc >= 4) {
        frameRate = std::atoi(argv[3]);
        frameRate = frameRate == 0 ? 60 : frameRate;
    }*/

    sf::RenderWindow win(sf::VideoMode::getDesktopMode(), "Gameboy Emulator", sf::Style::Default);
    //win.setVerticalSyncEnabled(true);
    
    // Initialize ImGui-SFML
    int init = ImGui::SFML::Init(win);
    ImGui::GetIO().FontGlobalScale = 2.5f;

    Gameboy g(romPath, bootRomPath, win.getSize());

    //printf("Memtype: %d\n", g.mem.memType);

    //const int MCYCLES_PER_FRAME = 69905;
    const int MCYCLES_PER_FRAME = 17556;

    // ImGui state
    sf::Clock deltaClock;
    float deltaT = 0.f;

    std::string lastPath = ".";

    while (win.isOpen()) { if (framePassed(deltaT, g.FPS * 60, true)) {
        // Update ImGui
        ImGui::SFML::Update(win, deltaClock.restart());
        
        handleEvents(win, g);

        // ImGui Window for settings
        ImGui::Begin("Emulator Settings");

        // LOAD STATE BUTTON
        if (ImGui::Button("Load State")) {
            IGFD::FileDialogConfig config;
            config.path = lastPath;
            ImGuiFileDialog::Instance()->OpenDialog("LoadStateDlg", 
                "Choose Save State", ".sav,.state", config);
        }

        ImGui::SameLine();

        // SAVE STATE BUTTON
        if (ImGui::Button("Save State")) {
            IGFD::FileDialogConfig config;
            config.path = lastPath;
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
                size_t pos = filePath.find_last_of("/\\");
                std::string dir = (pos != std::string::npos) ? filePath.substr(0, pos + 1) : "";
                lastPath = dir;
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
                    size_t pos = filePath.find_last_of("/\\");
                    std::string dir = (pos != std::string::npos) ? filePath.substr(0, pos + 1) : "";
                    lastPath = dir;
                } else {
                    printf("Failed to save state to: %s\n", filePath.c_str());
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::PushID("GameSpeed");
        ImGui::Text("Game Speed: %d", int(g.FPS));
        ImGui::SameLine();
        if (ImGui::Button("+")) {
            g.FPS++;
        }
        ImGui::SameLine();
        if (ImGui::Button("-")) {
            g.FPS = std::max(1, g.FPS - 1);  // prevent going below 1
        }
        ImGui::PopID();

        ImGui::PushID("Scale");
        ImGui::Text("Scale: %d", int(g.ppu.scale));
        ImGui::SameLine();
        if (ImGui::Button("+")) {
            g.ppu.scale += 1.0;
            g.ppu.displaySprite.setScale({g.ppu.scale, g.ppu.scale});
        }
        ImGui::SameLine();
        if (ImGui::Button("-")) {
            g.ppu.scale = std::max(1.0, g.ppu.scale - 1.0);  // prevent going below 1
            g.ppu.displaySprite.setScale({g.ppu.scale, g.ppu.scale});
        } 
        ImGui::PopID();
        ImGui::End();
        
        for (int i = 0; i < MCYCLES_PER_FRAME; i++) {
            g.FDE();

            // One M Cycle == 4 T Cycles
            for (int j = 0; j < 4; j++) {
                g.ppu.main();
                g.timer();
            }
        }

        if (!win.isOpen()) { continue; }

        win.clear(sf::Color::Black);

        g.ppu.drawToScreen(win);

        ImGui::SFML::Render(win);

        win.display();
    }}

    // Cleanup ImGui
    ImGui::SFML::Shutdown();
}
