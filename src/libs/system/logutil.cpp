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

#include "reone/system/collectionutil.h"

namespace reone {

BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", LogChannel)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogSeverity)

typedef boost::log::sources::severity_channel_logger<LogSeverity, LogChannel> SeverityChannelLogger;

static const std::unordered_map<LogSeverity, std::string> kLogSeverityToName {
    {LogSeverity::Error, "ERR"},
    {LogSeverity::Warn, "WRN"},
    {LogSeverity::Info, "INF"},
    {LogSeverity::Debug, "DBG"}};

static LogSeverity g_minSeverity = LogSeverity::None;

static SeverityChannelLogger g_loggerGeneral;
static SeverityChannelLogger g_loggerResources;
static SeverityChannelLogger g_loggerResources2;
static SeverityChannelLogger g_loggerGraphics;
static SeverityChannelLogger g_loggerAudio;
static SeverityChannelLogger g_loggerGUI;
static SeverityChannelLogger g_loggerPerception;
static SeverityChannelLogger g_loggerConversation;
static SeverityChannelLogger g_loggerCombat;
static SeverityChannelLogger g_loggerScript;
static SeverityChannelLogger g_loggerScript2;
static SeverityChannelLogger g_loggerScript3;

static std::set<LogChannel> g_channels;

static std::map<LogChannel, SeverityChannelLogger *> g_channelToLogger {
    {LogChannel::General, &g_loggerGeneral},           //
    {LogChannel::Resources, &g_loggerResources},       //
    {LogChannel::Resources2, &g_loggerResources2},     //
    {LogChannel::Graphics, &g_loggerGraphics},         //
    {LogChannel::Audio, &g_loggerAudio},               //
    {LogChannel::GUI, &g_loggerGUI},                   //
    {LogChannel::Perception, &g_loggerPerception},     //
    {LogChannel::Conversation, &g_loggerConversation}, //
    {LogChannel::Combat, &g_loggerCombat},             //
    {LogChannel::Script, &g_loggerScript},             //
    {LogChannel::Script2, &g_loggerScript2},           //
    {LogChannel::Script3, &g_loggerScript3},           //
};

static std::string describeLogSeverity(LogSeverity severity) {
    return getFromLookupOrElse(kLogSeverityToName, severity, [&severity]() {
        return std::to_string(static_cast<int>(severity));
    });
}

static std::ostream &operator<<(std::ostream &stream, LogSeverity severity) {
    stream << describeLogSeverity(severity);
    return stream;
}

void initLog(LogSeverity minSeverity,
             std::set<LogChannel> channels,
             std::string filename) {
    g_minSeverity = minSeverity;
    g_channels = channels;

    boost::log::core::get()->set_filter(severity >= minSeverity);
    if (!filename.empty()) {
        boost::log::add_file_log(
            boost::log::keywords::file_name = filename,
            boost::log::keywords::format = (boost::log::expressions::stream << severity << " " << boost::log::expressions::smessage));
    } else {
        boost::log::add_console_log(
            std::clog,
            boost::log::keywords::format = (boost::log::expressions::stream << severity << " " << boost::log::expressions::smessage));
    }
    boost::log::add_common_attributes();
}

static void log(LogSeverity severity, const std::string &s, LogChannel channel) {
    if (!isLogSeverityEnabled(severity)) {
        return;
    }
    if (!isLogChannelEnabled(channel)) {
        return;
    }
    auto &logger = *g_channelToLogger.at(channel);
    BOOST_LOG_SEV(logger, severity) << s;
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

bool isLogSeverityEnabled(LogSeverity severity) {
    return severity >= g_minSeverity;
}

bool isLogChannelEnabled(LogChannel channel) {
    return g_channels.count(channel) > 0;
}

} // namespace reone
