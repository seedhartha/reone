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

enum class SeekOrigin {
    Begin,
    Current,
    End
};

enum class LogSeverity {
    Debug,
    Info,
    Warn,
    Error,

    None
};

enum class LogChannel {
    Global = 1,
    Resources = 2,
    Resources2 = 4,
    Graphics = 8,
    Audio = 16,
    GUI = 32,
    Perception = 64,
    Conversation = 128,
    Combat = 256,
    Script = 512,
    Script2 = 1024,
    Script3 = 2048
};

using ByteBuffer = std::vector<char>;

} // namespace reone
