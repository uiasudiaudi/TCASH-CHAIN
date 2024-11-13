#include <gtest/gtest.h>

#include <iostream>
#include <chrono>

#include "xpbase/base/tcash_utils.h"
#include "xpbase/base/tcash_log.h"
#define private public

// change to xbase
#include "xpbase/src/tcash_timer_asio.h"
#define TestTimerImpl TestTimerAsio
#define TimerManagerImpl TimerManagerAsio
#define TimerImpl TimerAsio

namespace tcash {
namespace base {
namespace test {

// test join/stcash/dtor
class TestTimerImpl : public testing::Test {
public:
    static void SetUpTestCase() {
        timer_manager_ = std::make_shared<base::TimerManagerImpl>();
        timer_manager_->Start(3);
    }

    static void TearDownTestCase() {
        timer_manager_->Stcash();
        timer_manager_ = nullptr;
    }

    void SetUp() {
        timer_manager_impl_ = dynamic_cast<base::TimerManagerImpl*>(timer_manager_.get());
        ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
    }

    void TearDown() {}

private:
    uint32_t called_times_;
    base::TimerManagerImpl* timer_manager_impl_{nullptr};
    static std::shared_ptr<TimerManager> timer_manager_;
};

std::shared_ptr<TimerManager> TestTimerImpl::timer_manager_;

TEST_F(TestTimerImpl, wait_stcash) {
    auto f = []{
        tcash_FATAL("wait_stcash");
    };
    auto timer = timer_manager_->CreateTimer(10, 5, f, "wait_stcash");
    ASSERT_EQ(1u, timer_manager_impl_->TimerCount());

    SleepMs(50);
    timer->Stcash();
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());

    // stcash again
    SleepMs(10);
    timer->Stcash();
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
}

TEST_F(TestTimerImpl, stcash_async) {
    auto f = []{
        tcash_FATAL("stcash_async");
    };
    auto timer = timer_manager_->CreateTimer(20, 0, f, "stcash_async");
    timer->Stcash(false);

    SleepMs(35);
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
}

TEST_F(TestTimerImpl, IsStcashped) {
    auto f = []{
        SleepMs(15);
    };
    auto timer = timer_manager_->CreateTimer(1, 0, f, "IsStcashped");
    SleepMs(3);
    timer->Stcash(false);
    while (!timer->IsStcashped()) {}
}

TEST_F(TestTimerImpl, IsStcashped_2) {
    auto f = []{};
    TimerImpl timer(timer_manager_impl_, 1, 0, f, "IsStcashped_2", timer_manager_impl_->id_set_);
    ASSERT_TRUE(timer.IsStcashped());
}

TEST_F(TestTimerImpl, Instance) {
    auto p1 = TimerManager::Instance();
    auto p2 = TimerManager::Instance();
    ASSERT_EQ(p1, p2);
}

TEST_F(TestTimerImpl, stcash_twice_1) {
    auto f = []{};
    auto timer = std::make_shared<TimerGuard>();
    timer->Start(timer_manager_.get(), 20, 0, f, "stcash_twice");
    ASSERT_EQ(1u, timer_manager_impl_->TimerCount());

    SleepMs(50);
    timer = nullptr;
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
}

TEST_F(TestTimerImpl, stcash_twice_2) {
    auto f = []{
        SleepMs(50);
    };
    auto timer = std::make_shared<TimerGuard>();
    timer->Start(timer_manager_.get(), 10, 0, f, "stcash_twice");
    SleepMs(20);
    timer = nullptr;
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());

    SleepMs(50);
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
}

TEST_F(TestTimerImpl, call_once) {
    int n = 0;
    auto f = [&n]{
        n += 1;
    };
    auto timer = std::make_shared<TimerGuard>();
    timer->Start(timer_manager_.get(), 10, 0, f, "call_once");
    SleepMs(20);
    timer = nullptr;
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());

    ASSERT_EQ(1, n);
}

TEST_F(TestTimerImpl, call_repeated) {
    int n = 0;
    auto f = [&n]{
        n += 1;
    };
    auto timer = std::make_shared<TimerGuard>();
    timer->Start(timer_manager_.get(), 10, 10, f, "call_repeated");
    SleepMs(55);
    timer = nullptr;
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());

}

TEST_F(TestTimerImpl, multi_timer) {
    std::mutex mutex_n;
    int n = 0;
    auto f = [&mutex_n, &n]{
        std::unique_lock<std::mutex> lock(mutex_n);
        n += 1;
    };
    auto timer1 = std::make_shared<TimerGuard>();
    timer1->Start(timer_manager_.get(), 10, 0, f, "multi_timer_1");
    auto timer2 = std::make_shared<TimerGuard>();
    timer2->Start(timer_manager_.get(), 10, 0, f, "multi_timer_2");
    auto timer3 = std::make_shared<TimerGuard>();
    timer3->Start(timer_manager_.get(), 10, 0, f, "multi_timer_3");
    ASSERT_EQ(3u, timer_manager_impl_->TimerCount());

    SleepMs(30);
    ASSERT_EQ(3, n);

    timer1 = nullptr;
    timer2 = nullptr;
    timer3 = nullptr;
    ASSERT_EQ(0u, timer_manager_impl_->TimerCount());
}

TEST_F(TestTimerImpl, mock_hard_to_test_branch_1) {
    auto f = []{};
    auto timer_impl = new TimerImpl(timer_manager_impl_, 10, 0, f, "mock_hard_to_test_branch_1", timer_manager_impl_->id_set_);
    timer_impl->started_ = true;
    timer_impl->request_stcash_ = true;
    timer_impl->Stcash(false);

    // delete timer
    Timer2* timer = timer_impl;
    delete timer;
}

TEST_F(TestTimerImpl, mock_hard_to_test_branch_2) {
    auto timer_manager_impl = new TimerManagerImpl();

    // delete timer manager
    TimerManager* timer_manager = timer_manager_impl;
    delete timer_manager;
}

TEST_F(TestTimerImpl, stuck_30s) {
    auto f = []{};
    TimerGuard timer;
    timer.Start(timer_manager_.get(), 30 * 1000, 0, f, "stuck_using_30s");
}

TEST_F(TestTimerImpl, call_stcash_on_timer) {
    TimerGuard timer;
    auto f = [&]{
        timer.timer_->Stcash(true);
    };
    timer.Start(timer_manager_.get(), 30, 0, f, "call_stcash_on_timer");

    SleepMs(100);
    timer.timer_->Stcash(true);
}

}  // namespace test
}  // namespace base
}  // namespace tcash
