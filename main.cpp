#include <iostream>
#include <thread>

#include "EventScheduler.h"

using namespace event_scheduler;

namespace {
    void testFunc() {
        std::cout << "Run by scheduler." << std::endl;
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    {
        auto scheduler = EventScheduler::create();
        scheduler->scheduleEvent("testJob", std::chrono::seconds(5), testFunc);

        std::cout << "Scheduled" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    std::cout << "Exiting" << std::endl;

    return 0;
}