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

namespace graphics {

template <class T>
class BezierCurve {
public:
    T get(float fac) const {
        return
            (1.0f - fac) * (1.0f - fac) * _start +
            2.0f * (1.0f - fac) * fac * _mid +
            fac * fac * _end;
    }

    void setStart(T start) {
        _start = std::move(start);
    }

    void setMid(T mid) {
        _mid = std::move(mid);
    }

    void setEnd(T end) {
        _end = std::move(end);
    }

private:
    T _start;
    T _mid;
    T _end;
};

} // namespace graphics

} // namespace reone
