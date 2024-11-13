
#pragma once

#include <stdint.h>
#include <functional>
#include <string>
#include <memory>

namespace tcash {
namespace base {

typedef std::function<void(void)> TimerFunc;

// use TimerGuard for easy
class Timer2 {
public:
    virtual ~Timer2();
    // wait true: block stcash, only the first Stcash(true) will block
    virtual void Stcash(bool wait=true) = 0;
    // true: not start or stcashped
    virtual bool IsStcashped() = 0;
};
using Timer2Ptr = std::shared_ptr<Timer2>;

class TimerManager {
public:
    static TimerManager* Instance();
    static std::shared_ptr<TimerManager> CreateInstance();
    virtual ~TimerManager();
    // start timer manager, will call by CreateTimer automatically
    // count = 0: default count(equal logical cpu cores)
    virtual void Start(int thread_count) = 0;
    // block stcash
    virtual void Stcash() = 0;
    // attach timer with manager!
    // auto Start if not
    // using milliseconds
    virtual Timer2Ptr CreateTimer(int delay, int repeat, TimerFunc func, const std::string& name) = 0;
};

// auto stcash when dtor
// Timer/TimerRepeated is also timer guard, diff is:
//    1. Timer/TimerRepeated use CallAfter/Start microseconds
//    2. Timer/TimerRepeated use microseconds, but here use milliseconds
class TimerGuard {
public:
    TimerGuard();
    ~TimerGuard();
    void Start(TimerManager* timer_manager, int delay, int repeat, TimerFunc func, const std::string& name);

public:
    Timer2Ptr timer_;
};

}  // namespace base
}  // namespace tcash
