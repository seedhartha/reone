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

#include "../src/common/hexutil.h"
#include "../src/common/types.h"

inline std::string describe(const std::string &s) {
    return reone::hexify(s);
}

inline std::string describe(const std::u16string &s) {
    return reone::hexify(s);
}

inline std::string describe(const reone::ByteArray &ba) {
    return reone::hexify(ba);
}

inline std::string describe(const glm::vec3 &v) {
    std::ostringstream ss;
    ss << "[";
    ss << v.x;
    ss << ", ";
    ss << v.y;
    ss << ", ";
    ss << v.z;
    ss << "]";
    return ss.str();
}

inline std::string describe(const glm::quat &q) {
    std::ostringstream ss;
    ss << "[";
    ss << q.w;
    ss << ", ";
    ss << q.x;
    ss << ", ";
    ss << q.y;
    ss << ", ";
    ss << q.z;
    ss << "]";
    return ss.str();
}

template <class T>
inline std::string notEqualMessage(const T &l, const T &r) {
    return describe(l) + " != " + describe(r);
}
