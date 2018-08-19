//
// Created by leoz on 8/19/18.
//

#ifndef EVENT_SCHEDULER_EVENTSCHEDULER_H
#define EVENT_SCHEDULER_EVENTSCHEDULER_H


#include <functional>
#include <chrono>


namespace event_scheduler {


class EventScheduler {
public:
    typedef std::function<void()> EventFunc;

    EventScheduler() = default;
    virtual ~EventScheduler() = default;

    virtual void scheduleEvent(const std::string& eventName, const std::chrono::milliseconds& interval,
                               EventFunc event) = 0;

    virtual void cancelEvent(const std::string& eventName) = 0;

    static std::shared_ptr<EventScheduler> create(size_t capacity = 100);

    // NO SUPPORTED
    EventScheduler(const EventScheduler&) = delete;
    EventScheduler& operator=(const EventScheduler&) = delete;
};

}


#endif //EVENT_SCHEDULER_EVENTSCHEDULER_H
