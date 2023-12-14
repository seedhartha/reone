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

#include "checkutil.h"
#include "types.h"

namespace reone {

class Logger {
public:
    static constexpr int kBufferSize = 512;

    static Logger instance;

    void init(LogSeverity minSeverity,
              std::set<LogChannel> enabledChannels,
              std::optional<std::string> filename);

    void append(std::string message,
                LogChannel channel,
                LogSeverity severity);

    bool isChannelEnabled(LogChannel channel) const {
        return _enabledChannels.count(channel) > 0;
    }

private:
    static thread_local std::optional<std::reference_wrapper<std::ostringstream>> buffer;

    LogSeverity _minSeverity {LogSeverity::Warn};
    std::set<LogChannel> _enabledChannels;
    std::unique_ptr<std::ostream> _stream;
    bool _inited {false};

    std::list<std::ostringstream> _buffers;

    std::mutex _buffersMutex;
    std::mutex _streamMutex;

    Logger() = default;

    ~Logger() {
        deinit();
    }

    void deinit();
    void flush(std::ostringstream &buffer);
};

} // namespace reone
