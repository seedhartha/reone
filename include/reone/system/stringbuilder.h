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

namespace reone {

class StringBuilder {
public:
    void clear() {
        _stream.str("");
        _stream.clear();
    }

    StringBuilder &append(char ch) {
        _stream << ch;
        return *this;
    }

    StringBuilder &append(char ch, int times) {
        _stream << std::string(times, ch);
        return *this;
    }

    StringBuilder &append(const char *str) {
        _stream << str;
        return *this;
    }

    StringBuilder &append(const char *str, int len) {
        _stream << std::string(str, len);
        return *this;
    }

    StringBuilder &append(const std::string &s) {
        _stream << s;
        return *this;
    }

    std::string string() {
        return _stream.str();
    }

    bool empty() {
        auto pos = _stream.tellp();
        _stream.seekp(0, std::ios::end);
        auto empty = _stream.tellp() == 0;
        _stream.seekp(pos);
        return empty;
    }

private:
    std::ostringstream _stream;
};

} // namespace reone
