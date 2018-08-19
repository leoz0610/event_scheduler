//
// Created by leoz on 8/19/18.
//

#include <gtest/gtest.h>
#include "../EventScheduler.h"

#include <thread>


using namespace ::testing;

namespace event_scheduler {

    class EventSchedulerFixture : public Test {
    public:
        typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

        EventSchedulerFixture()
        : d_scheduler()
        {}


        void expectEvent(const TimePoint& begin, const std::chrono::milliseconds& expectedInterval, bool* isCalled) {
            auto end = std::chrono::system_clock::now();
            auto actualInterval = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            EXPECT_EQ(expectedInterval, actualInterval);
            *isCalled = true;
        }

    protected:
        std::shared_ptr<EventScheduler> d_scheduler;
    };


    TEST_F(EventSchedulerFixture, scheduleEventHappyPathTest) {
        d_scheduler = EventScheduler::create();
        auto begin = std::chrono::system_clock::now();
        std::chrono::milliseconds interval(10);
        bool isCalled(false);

        d_scheduler->scheduleEvent("testEvent", interval, [&]() { expectEvent(begin, interval, &isCalled); });
        EXPECT_FALSE(isCalled);

        std::this_thread::sleep_for(interval);
        EXPECT_TRUE(isCalled);
    }

    TEST_F(EventSchedulerFixture, scheduleEventDuplicateTest) {
        d_scheduler = EventScheduler::create();
        auto begin = std::chrono::system_clock::now();
        std::chrono::milliseconds interval(10);
        bool isCalled1(false);
        bool isCalled2(false);

        d_scheduler->scheduleEvent("testEvent", interval, [&]() { expectEvent(begin, interval, &isCalled1); });
        EXPECT_FALSE(isCalled1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_THROW(d_scheduler->scheduleEvent("testEvent", interval,
                                                [&]() { expectEvent(begin, interval, &isCalled2); }), std::logic_error);

        std::this_thread::sleep_for(interval);
        EXPECT_TRUE(isCalled1);
        EXPECT_FALSE(isCalled2);
    }

    TEST_F(EventSchedulerFixture, scheduleEventMultipleInOrderTest) {
        d_scheduler = EventScheduler::create();
        auto begin1 = std::chrono::system_clock::now();
        std::chrono::milliseconds interval1(10);
        std::string eventName1("testEvent1");
        bool isCalled1(false);

        // schedule 1st
        d_scheduler->scheduleEvent(eventName1, interval1, [&]() { expectEvent(begin1, interval1, &isCalled1); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto begin2 = std::chrono::system_clock::now();
        std::chrono::milliseconds interval2(20);
        std::string eventName2("testEvent2");
        bool isCalled2(false);

        // schedule 2nd
        d_scheduler->scheduleEvent(eventName2, interval2, [&]() { expectEvent(begin2, interval2, &isCalled2); });

        std::this_thread::sleep_for(interval1);
        EXPECT_TRUE(isCalled1);
        EXPECT_FALSE(isCalled2);

        std::this_thread::sleep_for(interval2 - interval1);
        EXPECT_TRUE(isCalled2);
    }

    TEST_F(EventSchedulerFixture, scheduleEventMultipleOutOfOrderTest) {
        d_scheduler = EventScheduler::create();
        auto begin1 = std::chrono::system_clock::now();
        std::chrono::milliseconds interval1(20);
        std::string eventName1("testEvent1");
        bool isCalled1(false);

        // schedule 1st
        d_scheduler->scheduleEvent(eventName1, interval1, [&]() { expectEvent(begin1, interval1, &isCalled1); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto begin2 = std::chrono::system_clock::now();
        std::chrono::milliseconds interval2(10);
        std::string eventName2("testEvent2");
        bool isCalled2(false);

        // schedule 2nd
        d_scheduler->scheduleEvent(eventName2, interval2, [&]() { expectEvent(begin2, interval2, &isCalled2); });

        std::this_thread::sleep_for(interval2);
        EXPECT_FALSE(isCalled1);
        EXPECT_TRUE(isCalled2);

        std::this_thread::sleep_for(interval1 - interval2);
        EXPECT_TRUE(isCalled1);
    }

    TEST_F(EventSchedulerFixture, cancelEventHappyPathTest) {
        d_scheduler = EventScheduler::create();
        auto begin = std::chrono::system_clock::now();
        std::chrono::milliseconds interval(10);
        std::string eventName("testEvent");
        bool isCalled(false);

        d_scheduler->scheduleEvent(eventName, interval, [&]() { expectEvent(begin, interval, &isCalled); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        d_scheduler->cancelEvent(eventName);

        std::this_thread::sleep_for(2 * interval);
        EXPECT_FALSE(isCalled);
    }

    TEST_F(EventSchedulerFixture, cancelEventNotExistTest) {
        d_scheduler = EventScheduler::create();

        EXPECT_THROW(d_scheduler->cancelEvent("testName"), std::out_of_range);
    }

    TEST_F(EventSchedulerFixture, cancelEventDuplicateTest) {
        d_scheduler = EventScheduler::create();
        auto begin = std::chrono::system_clock::now();
        std::chrono::milliseconds interval(10);
        std::string eventName("testEvent");
        bool isCalled(false);

        d_scheduler->scheduleEvent(eventName, interval, [&]() { expectEvent(begin, interval, &isCalled); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        d_scheduler->cancelEvent(eventName);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_THROW(d_scheduler->cancelEvent(eventName), std::out_of_range);

        EXPECT_FALSE(isCalled);
    }

}