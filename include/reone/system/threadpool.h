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

#pragma once

namespace reone {

using TaskFunc = std::function<void(const std::atomic_bool &canceled)>;

class Task : boost::noncopyable {
public:
    Task(TaskFunc func) :
        _func(std::move(func)) {
    }

    inline void cancel() {
        _canceled = true;
    }

private:
    TaskFunc _func;
    std::atomic_bool _canceled {false};

    inline void operator()() {
        _func(_canceled);
    }

    friend class ThreadPool;
};

class IThreadPool {
public:
    virtual ~IThreadPool() = default;

    virtual std::shared_ptr<Task> enqueue(TaskFunc func) = 0;
};

class ThreadPool : public IThreadPool, boost::noncopyable {
public:
    ~ThreadPool() {
        deinit();
    }

    void init();
    void deinit();

    std::shared_ptr<Task> enqueue(TaskFunc func) override {
        std::lock_guard<std::mutex> lock(_mutex);
        auto task = std::make_shared<Task>(std::move(func));
        _tasks.push(task);
        _condVar.notify_one();
        return task;
    }

private:
    std::vector<std::thread> _threads;
    std::atomic_bool _running {false};

    std::queue<std::shared_ptr<Task>> _tasks;
    std::mutex _mutex;
    std::condition_variable _condVar;

    void workerThreadFunc() {
        while (_running) {
            std::shared_ptr<Task> task;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _condVar.wait(lock, [this]() { return !_running || !_tasks.empty(); });
                if (!_running) {
                    return;
                }
                task = _tasks.front();
                _tasks.pop();
            }
            if (task) {
                (*task)();
            }
        }
    }
};

} // namespace reone
