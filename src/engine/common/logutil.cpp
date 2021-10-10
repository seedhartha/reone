/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "logutil.h"

#include "collectionutil.h"
#include "threadutil.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

const char kLogFilename[] = "reone.log";

enum class LogLevel {
    Error,
    Warn,
    Info,
    Debug
};

static int g_logChannels = LogChannels::general;
static bool g_logToFile = false;
static mutex g_logMutex;

static unique_ptr<fs::ofstream> g_logFile;

static const unordered_map<LogLevel, string> g_nameByLogLevel {
    {LogLevel::Error, "ERR"},
    {LogLevel::Warn, "WRN"},
    {LogLevel::Info, "INF"},
    {LogLevel::Debug, "DBG"}};

static string describeLogLevel(LogLevel level) {
    return getFromLookupOrElse(g_nameByLogLevel, level, [&level]() {
        return to_string(static_cast<int>(level));
    });
}

static const unordered_map<int, string> g_nameByLogChannel {
    {LogChannels::general, "General"},
    {LogChannels::resources, "Resources"},
    {LogChannels::resources2, "Resources"},
    {LogChannels::graphics, "Graphics"},
    {LogChannels::audio, "Audio"},
    {LogChannels::gui, "GUI"},
    {LogChannels::conversation, "Conversation"},
    {LogChannels::combat, "Combat"},
    {LogChannels::script, "Script"},
    {LogChannels::script2, "Script"},
    {LogChannels::script3, "Script"}};

static string describeLogChannel(int channel) {
    return getFromLookupOrElse(g_nameByLogChannel, channel, [&channel]() {
        return to_string(channel);
    });
}

static void log(ostream &out, LogLevel level, const string &s, int channel) {
    boost::format msg(boost::format("%s [%s] %s: %s") %
                      describeLogLevel(level) %
                      getThreadName() %
                      describeLogChannel(channel) %
                      s);

    lock_guard<mutex> lock(g_logMutex);
    out << msg << endl;
}

static void log(LogLevel level, const string &s, int channel) {
    if (!isLogChannelEnabled(channel))
        return;

    if (g_logToFile && !g_logFile) {
        fs::path path(fs::current_path());
        path.append(kLogFilename);

        g_logFile = make_unique<fs::ofstream>(path);
    }

    auto &out = g_logToFile ? *g_logFile : cout;
    log(out, level, s, channel);
}

void error(const string &s, int channel) {
    log(LogLevel::Error, s, channel);
}

void error(const boost::format &s, int channel) {
    log(LogLevel::Error, str(s), channel);
}

void warn(const string &s, int channel) {
    log(LogLevel::Warn, s, channel);
}

void warn(const boost::format &s, int channel) {
    log(LogLevel::Warn, str(s), channel);
}

void info(const string &s, int channel) {
    log(LogLevel::Info, s, channel);
}

void info(const boost::format &s, int channel) {
    log(LogLevel::Info, str(s), channel);
}

void debug(const string &s, int channel) {
    log(LogLevel::Debug, s, channel);
}

void debug(const boost::format &s, int channel) {
    return debug(str(s), channel);
}

bool isLogChannelEnabled(int channel) {
    return g_logChannels & channel;
}

void setLogChannels(int mask) {
    g_logChannels = mask;
}

void setLogToFile(bool logToFile) {
    g_logToFile = logToFile;
}

} // namespace reone
