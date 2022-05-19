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

#include "unicodeutil.h"

#include <codecvt>

using namespace std;

namespace reone {

u16string convertUTF8ToUTF16(string s) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> conversion;
    return conversion.from_bytes(s);
}

string convertUTF16ToUTF8(u16string s) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> conversion;
    return conversion.to_bytes(s);
}

} // namespace reone
