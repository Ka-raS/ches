#include <cassert>

#include "thread.hpp"

namespace cheslib {

Thread::Thread()
    : _state{State::Waiting},
      _thread{&Thread::thread_loop, this} {}

Thread::~Thread() {
    _state.wait(State::Running, std::memory_order_acquire);
    _state.store(State::Terminating, std::memory_order_release);
    _state.notify_one();
    _thread.join();
}

Thread::State Thread::state() const {
    return _state.load(std::memory_order_acquire);
}

void Thread::wait_while_running() const {
    _state.wait(State::Running, std::memory_order_acquire);
}

void Thread::assign_job(std::function<void()> job) {
    assert(job);
    assert(_state.load(std::memory_order_acquire) == State::Waiting);

    _job = std::move(job);
    _state.store(State::Running, std::memory_order_release);
    _state.notify_one();
}

void Thread::thread_loop() {
    while (true) {
        switch (_state.load(std::memory_order_acquire)) {
        case State::Waiting:
            _state.wait(State::Waiting, std::memory_order_acquire);
            break;

        case State::Running:
            _job();
            _state.store(State::Waiting, std::memory_order_release);
            _state.notify_one();
            break;

        case State::Terminating:
            return;
        }
    }
}

} // namespace cheslib
