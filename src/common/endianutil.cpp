/*
 * Copyright (c) 2020 The reone project contributors
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

#include "endianutil.h"

#include <cstdint>
#include <cstring>

using namespace std;

namespace reone {

template <>
void swapBytes(uint16_t &val) {
    val = ((val >> 8) & 0xff) | ((val & 0xff) << 8);
}

template <>
void swapBytes(uint32_t &val) {
    val = ((val >> 24) & 0xff) |
        (((val >> 16) & 0xff) << 8) |
        (((val >> 8) & 0xff) << 16) |
        ((val & 0xff) << 24);
}

template <>
void swapBytes(uint64_t &val) {
    val = ((val >> 56) & 0xff) |
        (((val >> 48) & 0xff) << 8) |
        (((val >> 40) & 0xff) << 16) |
        (((val >> 32) & 0xff) << 24) |
        (((val >> 24) & 0xff) << 32) |
        (((val >> 16) & 0xff) << 40) |
        (((val >> 8) & 0xff) << 48) |
        ((val & 0xff) << 56);
}

template <>
void swapBytes(int16_t &val) {
    val = ((val >> 8) & 0xff) | ((val & 0xff) << 8);
}

template <>
void swapBytes(int32_t &val) {
    val = ((val >> 24) & 0xff) |
        (((val >> 16) & 0xff) << 8) |
        (((val >> 8) & 0xff) << 16) |
        ((val & 0xff) << 24);
}

template <>
void swapBytes(int64_t &val) {
    val = ((val >> 56) & 0xff) |
        (((val >> 48) & 0xff) << 8) |
        (((val >> 40) & 0xff) << 16) |
        (((val >> 32) & 0xff) << 24) |
        (((val >> 24) & 0xff) << 32) |
        (((val >> 16) & 0xff) << 40) |
        (((val >> 8) & 0xff) << 48) |
        ((val & 0xff) << 56);
}

template <>
void swapBytes(float &val) {
    uint32_t uintVal;
    memcpy(&uintVal, &val, 4);
    swapBytes(uintVal);
    memcpy(&val, &uintVal, 4);
}

template <>
void swapBytes(double &val) {
    uint64_t uintVal;
    memcpy(&uintVal, &val, 8);
    swapBytes(uintVal);
    memcpy(&val, &uintVal, 8);
}

} // namespace reone
