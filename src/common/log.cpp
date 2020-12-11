/*
 * Copyright (c) 2020 The reone project contributors
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

#include "log.h"

#include <iostream>
#include <memory>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace std;

namespace reone {

const char kLogFilename[] = "reone.log";

enum class LogLevel {
    Error,
    Warn,
    Info,
    Debug
};

static uint32_t g_debugLevel = 0;
static bool g_logToFile = false;

static std::unique_ptr<fs::ofstream> g_logFile;

inline static const char *describeLogLevel(LogLevel level) {
    switch (level) {
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Debug:
            return "DEBUG";
        default:
            throw invalid_argument("Invalid log level: " + to_string(static_cast<int>(level)));
    }
}

static void log(ostream &out, LogLevel level, const string &s) {
    boost::format msg(boost::format("%s %s") % describeLogLevel(level) % s);
    out << msg << endl;
}

static void log(LogLevel level, const string &s) {
    if (g_logToFile && !g_logFile) {
        fs::path path(fs::current_path());
        path.append(kLogFilename);

        g_logFile = make_unique<fs::ofstream>(path);
    }

    auto &out = g_logToFile ? *g_logFile : cout;
    log(out, level, s);
}

void error(const string &s) {
    log(LogLevel::Error, s);
}

void error(const boost::format &s) {
    log(LogLevel::Error, str(s));
}

void warn(const string &s) {
    log(LogLevel::Warn, s);
}

void warn(const boost::format &s) {
    log(LogLevel::Warn, str(s));
}

void info(const string &s) {
    log(LogLevel::Info, s);
}

void info(const boost::format &s) {
    log(LogLevel::Info, str(s));
}

void debug(const string &s, uint32_t level) {
    if (level <= getDebugLogLevel()) {
        log(LogLevel::Debug, s);
    }
}

void debug(const boost::format &s, uint32_t level) {
    return debug(str(s), level);
}

uint32_t getDebugLogLevel() {
    return g_debugLevel;
}

void setDebugLogLevel(uint32_t level) {
    g_debugLevel = level;
}

void setLogToFile(bool logToFile) {
    g_logToFile = logToFile;
}

} // namespace reone
