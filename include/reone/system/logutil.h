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

#include "logger.h"
#include "types.h"

namespace reone {

inline void error(const char *message, LogChannel channel = LogChannel::Global) {
    Logger::instance.append(message, channel, LogSeverity::Error);
}

inline void error(const std::string &message, LogChannel channel = LogChannel::Global) {
    error(message.c_str(), channel);
}

inline void warn(const char *message, LogChannel channel = LogChannel::Global) {
    Logger::instance.append(message, channel, LogSeverity::Warn);
}

inline void warn(const std::string &message, LogChannel channel = LogChannel::Global) {
    warn(message.c_str(), channel);
}

inline void info(const char *message, LogChannel channel = LogChannel::Global) {
    Logger::instance.append(message, channel, LogSeverity::Info);
}

inline void info(const std::string &message, LogChannel channel = LogChannel::Global) {
    info(message.c_str(), channel);
}

inline void debug(const char *message, LogChannel channel = LogChannel::Global) {
    Logger::instance.append(message, channel, LogSeverity::Debug);
}

inline void debug(const std::string &message, LogChannel channel = LogChannel::Global) {
    debug(message.c_str(), channel);
}

} // namespace reone
