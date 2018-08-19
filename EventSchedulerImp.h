//
// Created by leoz on 8/19/18.
//

#ifndef EVENT_SCHEDULER_EVENTSCHEDULERIMP_H
#define EVENT_SCHEDULER_EVENTSCHEDULERIMP_H


#include "EventScheduler.h"

#include <map>
#include <mutex>
#include <condition_variable>
#include <string>
#include <chrono>
#include <thread>


namespace event_scheduler {

class EventSchedulerImp : public EventScheduler {
public:
    explicit EventSchedulerImp(size_t capacity);
    ~EventSchedulerImp() override;

    void scheduleEvent(const std::string& eventName, const std::chrono::milliseconds& interval,
                       EventFunc event) override;

    void cancelEvent(const std::string& eventName) override;

private:
    typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

    struct EventNode {
        EventFunc event;
        const std::string* eventName; // points to the key of d_eventMap
        TimePoint timePoint;
    };

    std::map<TimePoint, std::shared_ptr<EventNode>> d_timedMap; // guard by d_mutex
    std::map<std::string, std::shared_ptr<EventNode>> d_eventMap; // guard by d_mutex
    std::mutex d_mutex;
    std::condition_variable d_enqueuedEvent; // guard by d_mutex
    std::condition_variable d_capacityFullEvent; // guard by d_mutex
    std::unique_ptr<std::thread> d_job_up;
    const size_t d_capacity;
    std::atomic<bool> d_isStopping;

    void processEvents();
};

}

#endif //EVENT_SCHEDULER_EVENTSCHEDULERIMP_H
