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

class ThreadPool {
public:
    ~ThreadPool() {
        deinit();
    }

    void init();
    void deinit();

    void enqueue(std::function<void()> block);

private:
    using Task = std::function<void()>;

    std::vector<std::thread> _threads;
    std::atomic_bool _running {false};

    std::queue<Task> _tasks;
    std::mutex _mutex;
    std::condition_variable _condVar;

    void workerThreadFunc();
};

} // namespace reone
