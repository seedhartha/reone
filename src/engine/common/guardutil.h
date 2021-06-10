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

template <class T>
inline void ensureNotNull(T obj, const std::string &name) {
    if (!obj) {
        throw std::invalid_argument(name + " must not be null");
    }
}

template <class T>
inline void ensureNotEmpty(T obj, const std::string &name) {
    if (obj.empty()) {
        throw std::invalid_argument(name + " must not be empty");
    }
}

} // namespace reone
