/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/system/threadpool.h"

namespace reone {

void ThreadPool::init() {
    for (auto i = 0; i < std::thread::hardware_concurrency(); ++i) {
        _threads.emplace_back(std::bind(&ThreadPool::workerThreadFunc, this));
    }
    _running = true;
}

void ThreadPool::deinit() {
    _running = false;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _condVar.notify_all();
    }
    for (auto &thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    _threads.clear();
}

void ThreadPool::enqueue(std::function<void()> block) {
    std::lock_guard<std::mutex> lock(_mutex);
    _tasks.emplace(block);
    _condVar.notify_one();
}

void ThreadPool::workerThreadFunc() {
    while (_running) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condVar.wait(lock, [this]() { return !_running || !_tasks.empty(); });
            if (!_running) {
                return;
            }
            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}

} // namespace reone
