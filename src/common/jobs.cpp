/*
 * Copyright (c) 2020 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "jobs.h"

#include <thread>

#include <boost/asio/post.hpp>

using namespace std;

namespace reone {

JobExecutor &JobExecutor::instance() {
    static JobExecutor executor;
    return executor;
}

JobExecutor::~JobExecutor() {
    deinit();
}

void JobExecutor::deinit() {
    _cancel = true;
    _pool.join();
}

void JobExecutor::enqueue(const function<void(const atomic_bool &)> &job) {
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
        this_thread::yield();
    }
}

} // namespace reone
