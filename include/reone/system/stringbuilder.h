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

#include <sstream>
#include <string>

namespace reone {

class StringBuilder {
public:
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

    StringBuilder &repeat(char ch, int times) {
        _stream << std::string(times, ch);
        return *this;
    }

    std::string build() {
        return _stream.str();
    }

private:
    std::ostringstream _stream;
};

} // namespace reone
