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

namespace graphics {

class IStatistic {
public:
    virtual ~IStatistic() = default;

    virtual void resetDrawCalls() = 0;
    virtual void incrementDrawCalls() = 0;
    virtual int numDrawCalls() const = 0;
};

class Statistic : public IStatistic, boost::noncopyable {
public:
    void resetDrawCalls() override {
        _numDrawCalls = 0;
    }

    void incrementDrawCalls() override {
        ++_numDrawCalls;
    }

    int numDrawCalls() const override {
        return _numDrawCalls;
    }

private:
    int _numDrawCalls {0};
};

} // namespace graphics

} // namespace reone
