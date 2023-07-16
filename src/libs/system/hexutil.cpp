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

#include "reone/system/hexutil.h"

namespace reone {

std::string hexify(const ByteBuffer &ba, std::string separator) {
    std::ostringstream ss;
    for (auto &ch : ba) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch & 0xff) << separator;
    }
    return ss.str();
}

std::string hexify(const std::string &s, std::string separator) {
    std::ostringstream ss;
    for (auto &ch : s) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch & 0xff) << separator;
    }
    return ss.str();
}

ByteBuffer unhexify(const std::string &s) {
    auto bytes = ByteBuffer();
    for (size_t i = 0; i < s.size(); i += 2) {
        uint8_t byte;
        sscanf(&s[i], "%02hhx", &byte);
        bytes.push_back(static_cast<char>(byte));
    }
    return bytes;
}

} // namespace reone
