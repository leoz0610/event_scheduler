//
// Created by leoz on 8/19/18.
//

#include <thread>
#include <iostream>
#include <algorithm>
#include "EventSchedulerImp.h"


namespace event_scheduler {

EventSchedulerImp::EventSchedulerImp(size_t capacity)
        : EventScheduler(),
          d_timedMap(),
          d_eventMap(),
          d_mutex(),
          d_enqueuedEvent(),
          d_capacityFullEvent(),
          d_job_up(),
          d_capacity(capacity),
          d_isStopping(false)
{
    d_job_up.reset(new std::thread(std::bind(&EventSchedulerImp::processEvents, this)));
}

EventSchedulerImp::~EventSchedulerImp() {
    d_isStopping = true;
    d_enqueuedEvent.notify_one();
    d_job_up->join();
}

void EventSchedulerImp::scheduleEvent(const std::string &eventName, const std::chrono::milliseconds &interval,
                                      EventFunc event)
{
    std::unique_lock<std::mutex> lck(d_mutex);

    while (d_eventMap.size() >= d_capacity) {
        d_capacityFullEvent.wait(lck);
    }

    if (d_eventMap.find(eventName) != d_eventMap.end()) {
        throw std::logic_error("Event is already scheduled.");
    }

    auto eventNode(std::make_shared<EventNode>());
    eventNode->event = event;
    auto iter = d_eventMap.emplace(eventName, eventNode).first;
    eventNode->eventName = &iter->first;

    TimePoint tPoint = std::chrono::system_clock::now() + interval;
    eventNode->timePoint = tPoint;
    d_timedMap.emplace(tPoint, eventNode);
    d_enqueuedEvent.notify_one();
}

void EventSchedulerImp::cancelEvent(const std::string &eventName) {
    std::unique_lock<std::mutex> lck(d_mutex);

    auto iter = d_eventMap.find(eventName);
    if (iter == d_eventMap.end()) {
        throw std::out_of_range("Event is not found.");
    }

    d_eventMap.erase(eventName);
    d_timedMap.erase(iter->second->timePoint);
}


void EventSchedulerImp::processEvents() {
    while (!d_isStopping) {
        std::unique_lock<std::mutex> lck(d_mutex);

        while (!d_isStopping && d_timedMap.empty()) {
            d_enqueuedEvent.wait(lck);
        }
        if (d_isStopping) {
            break;
        }
        auto front = d_timedMap.begin();
        auto delta = std::chrono::system_clock::now() - front->first;
        if (delta.count() < 0LL) {
            d_enqueuedEvent.wait_for(lck, delta);
            continue;
        }

        // execute the event;
        try {
            front->second->event();
        }
        catch (...) {
            std::cerr << "Event job throws some exceptions." << std::endl;
        }

        d_eventMap.erase(*front->second->eventName);
        d_timedMap.erase(front->first);
        d_capacityFullEvent.notify_one();
    }
}

}