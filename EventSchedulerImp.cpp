//
// Created by leoz on 8/19/18.
//

#include "EventSchedulerImp.h"


namespace event_scheduler {

EventSchedulerImp::EventSchedulerImp(size_t capacity)
        : EventScheduler()
{}

void EventSchedulerImp::scheduleEvent(const std::string &eventName, const std::chrono::milliseconds &interval,
                                      EventFunc event)
{

}

void EventSchedulerImp::cancelEvent(const std::string &eventName) {}

}