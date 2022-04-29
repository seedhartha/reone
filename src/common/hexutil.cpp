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

#include "hexutil.h"

using namespace std;

namespace reone {

string hexify(const string &s) {
    ostringstream ss;
    for (auto &ch : s) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(ch & 0xff) << " ";
    }
    return ss.str();
}

string hexify(const u16string &s) {
    ostringstream ss;
    for (auto &ch : s) {
        ss << hex << setw(4) << setfill('0') << static_cast<int>(ch & 0xffff) << " ";
    }
    return ss.str();
}

string hexify(const reone::ByteArray &ba) {
    ostringstream ss;
    for (auto &b : ba) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(b & 0xff) << " ";
    }
    return ss.str();
}

} // namespace reone
