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

template <class... Args>
class Command : boost::noncopyable {
public:
    using CanExecuteChangedHandler = std::function<void(bool)>;

    Command(std::function<void(Args...)> execute, bool canExecute = true) :
        _execute(std::move(execute)),
        _canExecute(canExecute) {
    }

    inline void execute(Args... args) {
        _execute(std::forward<Args>(args)...);
    }

    inline bool canExecute() const {
        return _canExecute;
    }

    void setCanExecute(bool canExecute) {
        _canExecute = canExecute;
        for (auto &handler : _canExecuteChangedHandlers) {
            handler(_canExecute);
        }
    }

    inline void addCanExecuteChangedHandler(CanExecuteChangedHandler handler) {
        _canExecuteChangedHandlers.push_back(std::move(handler));
    }

private:
    std::function<void(Args...)> _execute;
    bool _canExecute;
    std::list<CanExecuteChangedHandler> _canExecuteChangedHandlers;
};

} // namespace reone
