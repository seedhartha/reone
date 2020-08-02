#pragma once

#include <boost/asio/thread_pool.hpp>

namespace reone {

class JobExecutor {
public:
    static JobExecutor &instance();

    ~JobExecutor();

    void deinit();
    void enqueue(const std::function<void(const std::atomic_bool &)> &job);
    void cancel();
    void await();

private:
    boost::asio::thread_pool _pool;
    std::atomic_bool _cancel { false };
    std::atomic_int _jobsActive { 0 };

    JobExecutor() = default;
};

} // namespace reone
