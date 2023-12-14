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

#include "reone/system/logger.h"
#include "reone/system/threadutil.h"

namespace reone {

static const std::unordered_map<LogSeverity, std::string> kSeverityToName {
    {LogSeverity::Error, "ERROR"},
    {LogSeverity::Warn, "WARN"},
    {LogSeverity::Info, "INFO"},
    {LogSeverity::Debug, "DEBUG"}};

static const std::unordered_map<LogChannel, std::string> kChannelToName {
    {LogChannel::Global, "global"},
    {LogChannel::Resources, "resources"},
    {LogChannel::Resources2, "resources"},
    {LogChannel::Graphics, "graphics"},
    {LogChannel::Audio, "audio"},
    {LogChannel::GUI, "gui"},
    {LogChannel::Perception, "perception"},
    {LogChannel::Conversation, "conversation"},
    {LogChannel::Combat, "combat"},
    {LogChannel::Script, "script"},
    {LogChannel::Script2, "script"},
    {LogChannel::Script3, "script"}};

Logger Logger::instance;
thread_local std::optional<std::reference_wrapper<std::ostringstream>> Logger::buffer;

void Logger::init(LogSeverity minSeverity,
                  std::set<LogChannel> enabledChannels,
                  std::optional<std::string> filename) {
    checkThat(!_inited, "Logger already initialized");
    _minSeverity = minSeverity;
    _enabledChannels = std::move(enabledChannels);
    if (filename) {
        _stream = std::make_unique<std::ofstream>(*filename);
    } else {
        _stream = std::make_unique<std::ostream>(std::clog.rdbuf());
    }
    _inited = true;
}

void Logger::deinit() {
    if (!_inited) {
        return;
    }
    std::lock_guard<std::mutex> lock {_buffersMutex};
    for (auto &buffer : _buffers) {
        flush(buffer);
    }
    _inited = false;
}

void Logger::append(std::string message,
                    LogChannel channel,
                    LogSeverity severity) {
    if (!_inited) {
        return;
    }
    if (static_cast<int>(severity) < static_cast<int>(_minSeverity)) {
        return;
    }
    if (_enabledChannels.count(channel) == 0) {
        return;
    }
    auto formatted = str(boost::format("%1$5s [%2%][%3%] %4%\n") %
                         kSeverityToName.at(severity) %
                         threadName() %
                         kChannelToName.at(channel) %
                         message);
    if (!buffer) {
        std::lock_guard<std::mutex> lock {_buffersMutex};
        buffer = _buffers.emplace_back();
    }
    auto &buf = buffer->get();
    buf.write(&formatted[0], formatted.length());
    if (buf.tellp() >= kBufferSize) {
        flush(buf);
    }
}

void Logger::flush(std::ostringstream &buffer) {
    if (!_stream) {
        return;
    }
    auto str = buffer.str();
    {
        std::lock_guard<std::mutex> lock {_streamMutex};
        _stream->write(&str[0], str.length());
    }
    buffer.str("");
}

} // namespace reone
