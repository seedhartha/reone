#include "jobs.h"

#include <thread>

#include <boost/asio/post.hpp>

namespace reone {

JobExecutor &JobExecutor::instance() {
    static JobExecutor executor;
    return executor;
}

JobExecutor::~JobExecutor() {
    deinit();
}

void JobExecutor::deinit() {
    _pool.join();
}

void JobExecutor::enqueue(const std::function<void(const std::atomic_bool &)> &job) {
    _cancel = false;

    boost::asio::post(_pool, [&, job]() {
        ++_jobsActive;
        job(_cancel);
        --_jobsActive;
    });
}

void JobExecutor::cancel() {
    _cancel = true;
}

void JobExecutor::await() {
    while (_jobsActive) {
        std::this_thread::yield();
    }
}

} // namespace reone
