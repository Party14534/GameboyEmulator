# Gameboy Emulator

## Description
This Gameboy Emulator allows you to play most games for the original Gameboy handheld console. This emulator also supports savestates so you can save your progress when playing games at any point. You are also able to increase the speed that the emulator runs at.

## How to install
1. Ensure the latest version of CMake is installed
2. Clone the project repo
    ```shell
    git clone https://github.com/Party14534/GameboyEmulator.git
    ```

3. Move to the project's directory
    ```shell
    cd GameboyEmulator
    ```

4. Create a build folder and move into it
    ```shell
    mkdir build && cd build
    ```

5. Create a CMake build script
    ```shell
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```

6. Build the project
    ```shell
    cmake --build . --parallel
    ```

## How to use
Running the project differs slightly based on your operating system.
- Mac and Linux
    ```shell
    ./gameboy <path-to-rom> [optional: path-to-bootrom]
    ```
- Windows
    ```bash
    start Release/gameboy.exe <path-to-rom> [optional: path-to-bootrom]
    ```

### Controls
The gameboy has 8 different buttons, 4 directional buttons and 4 regular buttons.
- The directional buttons are mapped to the arrow keys.
- The B and A buttons are mapped to Z and X respectively.
- The Start and Select buttons are mapped to Enter and RShift respectively.

### Savestates
The emulator loads with a UI that has Save and Load buttons which allow you to search for specific locations on your computer to save your savestate or load a previously made savestate.

### Other features
The emulator also allows you to change the game speed and the scale of the gameboy screen that is displayed.

## Resources Used
Some articles that helped me create this emulator
- [Gameboy Bootstrap walkthrough](https://realboyemulator.wordpress.com/2013/01/03/a-look-at-the-game-boy-bootstrap-let-the-fun-begin/)
- [GBDev](gbdev.io)
- [Gameboy Technical Reference](https://gekkio.fi/files/gb-docs/gbctr.pdf)
- [Simple PPU](https://blog.tigris.fr/2019/09/15/writing-an-emulator-the-first-pixel/)
- [DAA](https://blog.ollien.com/posts/gb-daa/)
