/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/hexutil.h"

using namespace std;

namespace reone {

string hexify(const string &s, string separator) {
    ostringstream ss;
    for (auto &ch : s) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(ch & 0xff) << separator;
    }
    return ss.str();
}

ByteArray unhexify(const string &s) {
    auto bytes = ByteArray();
    for (size_t i = 0; i < s.size(); i += 2) {
        uint8_t byte;
        sscanf(&s[i], "%02hhx", &byte);
        bytes.push_back(static_cast<char>(byte));
    }
    return move(bytes);
}

} // namespace reone
