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

#include <cstdint>
#include <type_traits>

#include "types.h"

namespace reone {

Endianess getSystemEndianess();

/**
 * Inverts endianess of an integral or a floating point value.
 *
 * @param val value whose endianess to invert
 */
template <class T>
inline std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value> swapBytes(T &val) {
    uint8_t *b = reinterpret_cast<uint8_t *>(&val);
    for (size_t i = 0; i * 2 < sizeof(T); ++i) {
        std::swap(b[i], b[sizeof(T) - i - 1]);
    }
}

/**
 * Inverts endianess of an integral or a floating point value, if the specified
 * endianess differs from the system endianess.
 *
 * @param val value whose endianess to invert
 * @param other endianess to compare with the system endianess
 */
template <class T>
inline void swapBytesIfNotSystemEndianess(T &val, Endianess other) {
    if (other != getSystemEndianess()) {
        swapBytes(val);
    }
}

} // namespace reone
