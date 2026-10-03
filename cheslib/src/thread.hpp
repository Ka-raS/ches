#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace cheslib {

/// A job-reassignable thread that must have exactly one owner thread
class Thread {
  public:
    enum class State : uint8_t {
        Waiting,
        Running,
        Terminating
    };

  public:
    Thread() = default;
    const std::atomic<State> &state() const;
    void assign_job(std::function<void()> job);

    ~Thread();
    Thread(Thread &&) = delete;
    Thread(const Thread &) = delete;
    Thread &operator=(Thread &&) = delete;
    Thread &operator=(const Thread &) = delete;

  private:
    void thread_loop();

  private:
    std::function<void()> _job{};
    std::atomic<State> _state{State::Waiting};
    std::thread _thread{};

    static_assert(std::atomic<State>::is_always_lock_free);
};

} // namespace cheslib
