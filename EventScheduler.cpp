//
// Created by leoz on 8/19/18.
//

#include "EventScheduler.h"
#include "EventSchedulerImp.h"


namespace event_scheduler {

std::shared_ptr<EventScheduler> EventScheduler::create(size_t capacity) {
    return std::make_shared<EventSchedulerImp>(capacity);
}

}