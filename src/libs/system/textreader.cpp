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

#include "reone/system/textreader.h"

namespace reone {

std::optional<std::string> TextReader::readLine() {
    auto pos = _stream.position();

    std::vector<char> buf;
    buf.resize(256);
    int numRead = _stream.read(&buf[0], buf.size());
    if (numRead == 0) {
        return std::nullopt;
    }

    size_t len;
    for (len = 0; len < numRead; ++len) {
        if (buf[len] == '\r' || buf[len] == '\n') {
            break;
        }
    }
    if (buf[len] == '\r') {
        buf[len] = '\0';
        _stream.seek(pos + len + 2);
    } else if (buf[len] == '\n') {
        buf[len] = '\0';
        _stream.seek(pos + len + 1);
    } else {
        _stream.seek(pos + len);
    }

    return std::string(&buf[0], len);
}

} // namespace reone
