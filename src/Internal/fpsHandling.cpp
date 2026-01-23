#include "fpsHandling.h"

auto beginning = std::chrono::high_resolution_clock::now();
auto end = std::chrono::high_resolution_clock::now();
auto beginningSec = std::chrono::high_resolution_clock::now();
int tick = 1;
int fps = 0;

bool framePassed(float &deltaT, float framerateLimit, bool printFps){

    end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-beginning);
    auto durationSec = std::chrono::duration_cast<std::chrono::milliseconds>(end-beginningSec);
    if(durationSec.count() >= 1000){
        fps = tick;
        if(printFps) std::cout << fps << "\n";
        tick = 1;
        beginningSec = std::chrono::high_resolution_clock::now();
    }

    //only updates when 1 frame has passed
    if(duration.count() >= (1000.f/framerateLimit)){
        //updating time values
        deltaT = (duration.count())/1000.f;
        beginning = std::chrono::high_resolution_clock::now();
        tick++;
        return true;
    }

    return false;

}

