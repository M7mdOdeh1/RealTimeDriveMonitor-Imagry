#include "../include/ClockSystem.h"
#include <chrono>

ClockSystem::ClockSystem() : running(false), currentTime(0.0) {}

ClockSystem::~ClockSystem() {
    stop();
}

void ClockSystem::start(double tickInterval, std::function<void(double)> tickCallback) {
    running = true;
    clockThread = std::thread(&ClockSystem::clockLoop, this, tickInterval, tickCallback);
}

void ClockSystem::stop() {
    running = false;
    if (clockThread.joinable()) {
        clockThread.join();
    }
}

double ClockSystem::getCurrentTime() const {
    return currentTime;
}

void ClockSystem::clockLoop(double tickInterval, std::function<void(double)> tickCallback) {
    while (running) {
        tickCallback(currentTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(tickInterval)));
        std::lock_guard<std::mutex> lock(timeMutex);
        currentTime += tickInterval;
    }
}

void ClockSystem::setCurrentTime(double newTime) {
    std::lock_guard<std::mutex> lock(timeMutex);  
    currentTime = newTime;
}

