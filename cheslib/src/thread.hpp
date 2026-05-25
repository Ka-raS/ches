#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace cheslib {

class Thread {
  public:
    enum class State : uint8_t {
        Sleeping,
        Working,
        Closing
    };

  public:
    Thread();
    ~Thread();
    State state() const;
    void assign_job(std::function<void()> &&job);

  private:
    void thread_loop();

  private:
    std::function<void()> _job;
    std::atomic<State> _state;
    std::thread _thread;

    static_assert(std::atomic<State>::is_always_lock_free);
};

} // namespace cheslib
