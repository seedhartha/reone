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

#pragma once

namespace reone {

enum class SeekOrigin {
    Begin,
    Current,
    End
};

enum class OpenMode {
    Text,
    Binary
};

enum class LogLevel {
    None,
    Error,
    Warn,
    Info,
    Debug
};

struct LogChannels {
    static constexpr int general = 1;
    static constexpr int resources = 2;
    static constexpr int resources2 = 4;
    static constexpr int graphics = 8;
    static constexpr int audio = 16;
    static constexpr int gui = 32;
    static constexpr int perception = 64;
    static constexpr int conversation = 128;
    static constexpr int combat = 256;
    static constexpr int script = 512;
    static constexpr int script2 = 1024;
    static constexpr int script3 = 2048;
};

typedef std::string ByteArray;

} // namespace reone
