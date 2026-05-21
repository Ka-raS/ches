#include <cassert>

#include "thread.hpp"

namespace cheslib {

Thread::Thread()
    : _job(nullptr),
      _state(State::Sleeping),
      _thread(&Thread::thread_loop, this) {}

Thread::~Thread() {
    _state.wait(State::Working, std::memory_order_acquire);
    _state.store(State::Closing, std::memory_order_release);
    _state.notify_one();
    _thread.join();
}

Thread::State Thread::state() const {
    return _state.load(std::memory_order_acquire);
}

void Thread::assign_job(std::function<void()> &&job) {
    assert(job);
    assert(_state.load(std::memory_order_acquire) == State::Sleeping);

    _job = std::move(job);
    _state.store(State::Working, std::memory_order_release);
    _state.notify_one();
}

void Thread::thread_loop() {
    while (true) {
        switch (_state.load(std::memory_order_acquire)) {
        case State::Sleeping:
            _state.wait(State::Sleeping, std::memory_order_acquire);
            break;

        case State::Working:
            _job();
            _job = nullptr;
            _state.store(State::Sleeping, std::memory_order_release);
            _state.notify_one();
            break;

        case State::Closing:
            return;
        }
    }
}

} // namespace cheslib
