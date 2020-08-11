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

#pragma once

#include <string>

#include <boost/format.hpp>

namespace reone {

void setDebugLevel(uint32_t level);
uint32_t getDebugLevel();

void error(const std::string &s);
void error(const boost::format &s);
void warn(const std::string &s);
void warn(const boost::format &s);
void info(const std::string &s);
void info(const boost::format &s);
void debug(const std::string &s, uint32_t level = 1);
void debug(const boost::format &s, uint32_t level = 1);

} // namespace reone
