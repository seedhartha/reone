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

#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup.hpp>

namespace reone {

BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", LogChannel)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogSeverity)

typedef boost::log::sources::severity_channel_logger<LogSeverity, LogChannel> SeverityChannelLogger;

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

static LogSeverity g_minSeverity = LogSeverity::None;
static std::set<LogChannel> g_enabledChannels;

static std::unordered_map<LogChannel, SeverityChannelLogger> g_channelToLogger {
    {LogChannel::Global, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Global}},             //
    {LogChannel::Resources, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Resources}},       //
    {LogChannel::Resources2, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Resources2}},     //
    {LogChannel::Graphics, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Graphics}},         //
    {LogChannel::Audio, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Audio}},               //
    {LogChannel::GUI, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::GUI}},                   //
    {LogChannel::Perception, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Perception}},     //
    {LogChannel::Conversation, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Conversation}}, //
    {LogChannel::Combat, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Combat}},             //
    {LogChannel::Script, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Script}},             //
    {LogChannel::Script2, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Script2}},           //
    {LogChannel::Script3, SeverityChannelLogger {boost::log::keywords::channel = LogChannel::Script3}}            //
};

static std::ostream &operator<<(std::ostream &stream, LogSeverity severity) {
    stream << kSeverityToName.at(severity);
    return stream;
}

static std::ostream &operator<<(std::ostream &stream, LogChannel channel) {
    stream << kChannelToName.at(channel);
    return stream;
}

void initLog(LogSeverity minSeverity,
             std::set<LogChannel> enabledChannels,
             std::string filename) {
    g_minSeverity = minSeverity;
    g_enabledChannels = enabledChannels;

    boost::log::core::get()->set_filter(severity >= minSeverity);
    if (!filename.empty()) {
        boost::log::add_file_log(
            boost::log::keywords::file_name = filename,
            boost::log::keywords::format = (boost::log::expressions::stream << severity << " [" << channel << "] " << boost::log::expressions::smessage));
    } else {
        boost::log::add_console_log(
            std::clog,
            boost::log::keywords::format = (boost::log::expressions::stream << severity << " [" << channel << "] " << boost::log::expressions::smessage));
    }
    boost::log::add_common_attributes();
}

static void log(LogSeverity severity, const std::string &s, LogChannel channel) {
    if (!isLogChannelEnabled(channel)) {
        return;
    }
    BOOST_LOG_SEV(g_channelToLogger.at(channel), severity) << s;
}

void error(const std::string &s, LogChannel channel) {
    log(LogSeverity::Error, s, channel);
}

void error(const boost::format &s, LogChannel channel) {
    log(LogSeverity::Error, str(s), channel);
}

void warn(const std::string &s, LogChannel channel) {
    log(LogSeverity::Warn, s, channel);
}

void warn(const boost::format &s, LogChannel channel) {
    log(LogSeverity::Warn, str(s), channel);
}

void info(const std::string &s, LogChannel channel) {
    log(LogSeverity::Info, s, channel);
}

void info(const boost::format &s, LogChannel channel) {
    log(LogSeverity::Info, str(s), channel);
}

void debug(const std::string &s, LogChannel channel) {
    log(LogSeverity::Debug, s, channel);
}

void debug(const boost::format &s, LogChannel channel) {
    return debug(str(s), channel);
}

bool isLogChannelEnabled(LogChannel channel) {
    return g_enabledChannels.count(channel) > 0;
}

} // namespace reone
