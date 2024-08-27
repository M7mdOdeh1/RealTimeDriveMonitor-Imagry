#pragma once

#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

class ClockSystem {
public:
    ClockSystem();
    ~ClockSystem();

    void start(double tickInterval, std::function<void(double)> tickCallback);  
    void stop();
    double getCurrentTime() const;
    void setCurrentTime(double newTime);


private:
    void clockLoop(double tickInterval, std::function<void(double)> tickCallback);

    std::atomic<bool> running;
    std::thread clockThread;
    double currentTime;
    std::mutex timeMutex;

};