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

#include "reone/system/logutil.h"

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

static LogSeverity g_minSeverity = LogSeverity::Warn;
static std::set<LogChannel> g_enabledChannels;
static std::unique_ptr<std::ostream> g_stream;

void initLog(LogSeverity minSeverity,
             std::set<LogChannel> enabledChannels,
             std::optional<std::string> filename) {
    g_minSeverity = minSeverity;
    g_enabledChannels = enabledChannels;
    if (filename) {
        g_stream = std::make_unique<std::ofstream>(*filename);
    } else {
        g_stream = std::make_unique<std::ostream>(std::clog.rdbuf());
    }
}

bool isLogChannelEnabled(LogChannel channel) {
    return g_enabledChannels.count(channel) > 0;
}

void log(const char *message, LogChannel channel, LogSeverity severity) {
    if (static_cast<int>(severity) < static_cast<int>(g_minSeverity)) {
        return;
    }
    if (g_enabledChannels.count(channel) == 0) {
        return;
    }
    (*g_stream) << kSeverityToName.at(severity) << "[" << kChannelToName.at(channel) << "] " << message << std::endl;
}

} // namespace reone
