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

#pragma once

namespace reone {

class ITask;
class Task;

class Executor {
public:
    ~Executor();

    void init();
    void deinit();

    std::shared_ptr<ITask> submit(std::function<void()> fn);

    bool isRunning() const { return _running; }

private:
    volatile bool _running {false};
    std::vector<std::unique_ptr<std::thread>> _threads;
    std::queue<std::shared_ptr<Task>> _tasks;
    std::mutex _tasksMutex;

    void startThreads();
    void startThreads(int count);

    void poolThreadStart(int n);
    void runSingleTask();

    std::shared_ptr<Task> getTask();
};

} // namespace reone
