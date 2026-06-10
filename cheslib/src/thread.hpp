#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace cheslib {

class Thread {
  public:
    enum class State {
        Waiting,
        Running,
        Terminating
    };

  public:
    Thread();
    State state() const;
    void wait_while_running() const;
    void assign_job(std::function<void()> job);

    ~Thread();
    Thread(Thread &&) = delete;
    Thread(const Thread &) = delete;
    Thread &operator=(Thread &&) = delete;
    Thread &operator=(const Thread &) = delete;

  private:
    void thread_loop();

  private:
    std::function<void()> _job;
    std::atomic<State> _state;
    std::thread _thread;

    static_assert(std::atomic<State>::is_always_lock_free);
};

} // namespace cheslib
