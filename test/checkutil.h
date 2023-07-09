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

#include "reone/system/hexutil.h"
#include "reone/system/types.h"

inline std::string describe(const std::string &s) {
    return str(boost::format("%s (%d)") % reone::hexify(s) % s.length());
}

inline std::string describe(const reone::ByteBuffer &ba) {
    return str(boost::format("%s (%d)") % reone::hexify(ba) % ba.size());
}

inline std::string describe(const glm::vec2 &v) {
    std::ostringstream ss;
    ss << "[";
    ss << v.x;
    ss << ", ";
    ss << v.y;
    ss << "]";
    return ss.str();
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
    return str(boost::format("expected:\n%s\nbut was:\n%s") % describe(l) % describe(r));
}
