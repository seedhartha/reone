/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "executor.h"

#include "../logutil.h"
#include "../threadutil.h"

#include "impl/task.h"

using namespace std;

namespace reone {

static constexpr int kMinNumThreads = 1;
static constexpr int kMaxNumThreads = 4;

Executor::~Executor() {
    deinit();
}

void Executor::init() {
    startThreads();
}

void Executor::deinit() {
    _running = false;
    for (auto &t : _threads) {
        t->join();
    }
    _threads.clear();
}

shared_ptr<ITask> Executor::submit(function<void()> fn) {
    auto task = make_shared<Task>(move(fn));

    lock_guard<mutex> lock(_tasksMutex);
    _tasks.push(task);

    return move(task);
}

void Executor::startThreads() {
    int numCPU = static_cast<int>(thread::hardware_concurrency());
    int numThreads = max(kMinNumThreads, min(kMaxNumThreads, numCPU - 2));
    startThreads(numThreads);
}

void Executor::startThreads(int count) {
    _running = true;
    for (int i = 0; i < count; ++i) {
        auto t = make_unique<thread>(bind(&Executor::poolThreadStart, this, i));
        _threads.push_back(move(t));
    }
}

void Executor::poolThreadStart(int n) {
    setThreadName(str(boost::format("pool-%d") % n));
    debug("Thread started");

    while (isRunning()) {
        runSingleTask();
        this_thread::yield();
    }

    debug("Thread stopped");
}

void Executor::runSingleTask() {
    auto task = getTask();
    if (task) {
        task->run();
    }
}

shared_ptr<Task> Executor::getTask() {
    lock_guard<mutex> lock(_tasksMutex);
    return !_tasks.empty() ? _tasks.front() : shared_ptr<Task>();
}

} // namespace reone
