#include "thread.hpp"

#include <cassert>

namespace cheslib {

Thread::~Thread() {
    if (!_thread.joinable()) {
        return;
    }
    _state.wait(State::Running, std::memory_order::acquire);
    _state.store(State::Terminating, std::memory_order::release);
    _state.notify_one();
    _thread.join();
}

const std::atomic<Thread::State> &Thread::state() const {
    return _state;
}

void Thread::assign_job(std::function<void()> job) {
    assert(job);
    assert(_state.load(std::memory_order::acquire) == State::Waiting);

    if (!_thread.joinable()) {
        _thread = std::thread(&Thread::thread_loop, this);
    }
    _job = std::move(job);
    _state.store(State::Running, std::memory_order::release);
    _state.notify_one();
}

void Thread::thread_loop() {
    while (true) {
        switch (_state.load(std::memory_order::acquire)) {
        case State::Waiting:
            _state.wait(State::Waiting, std::memory_order::acquire);
            break;

        case State::Running:
            _job();
            _state.store(State::Waiting, std::memory_order::release);
            _state.notify_one();
            break;

        case State::Terminating:
            return;
        }
    }
}

} // namespace cheslib
