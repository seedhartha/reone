#pragma once

#include <boost/format.hpp>

namespace reone {

void error(const std::string &s);
void error(const boost::format &s);
void warn(const std::string &s);
void warn(const boost::format &s);
void info(const std::string &s);
void info(const boost::format &s);
void debug(const std::string &s);
void debug(const boost::format &s);

} // namespace reone
