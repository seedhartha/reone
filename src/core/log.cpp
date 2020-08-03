/*
 * Copyright © 2020 Vsevolod Kremianskii
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

namespace reone {

enum class LogLevel {
    Error,
    Warn,
    Info,
    Debug
};

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
            throw std::logic_error("Invalid log level: " + std::to_string(static_cast<int>(level)));
    }
}

static void log(std::ostream &out, LogLevel level, const std::string &s) {
    boost::format msg(boost::format("%s %s") % describeLogLevel(level) % s);
    out << msg << std::endl;
}

void error(const std::string &s) {
    log(std::cerr, LogLevel::Error, s);
}

void error(const boost::format &s) {
    log(std::cerr, LogLevel::Error, str(s));
}

void warn(const std::string &s) {
    log(std::cout, LogLevel::Warn, s);
}

void warn(const boost::format &s) {
    log(std::cout, LogLevel::Warn, str(s));
}

void info(const std::string &s) {
    log(std::cout, LogLevel::Info, s);
}

void info(const boost::format &s) {
    log(std::cout, LogLevel::Info, str(s));
}

void debug(const std::string &s) {
    log(std::cout, LogLevel::Debug, s);
}

void debug(const boost::format &s) {
    log(std::cout, LogLevel::Debug, str(s));
}

} // namespace reone
