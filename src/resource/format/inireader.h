/*
 * Copyright (c) 2022 The reone project contributors
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

#include "../resourceprovider.h"
#include "../types.h"

namespace reone {

namespace resource {

class INIreader {

public:
  INIreader();
  void load(boost::filesystem::path path);

private:
  std::map<std::string, std::string> _soundOptions;
  std::map<std::string, std::string> _gameOptions;
  std::map<std::string, std::string> _moviesShown;
  std::map<std::string, std::string> _alias;
  std::map<std::string, std::string> _graphicsOptions;
  std::map<std::string, std::string> _autoPauseOptions;
  std::map<std::string, std::string> _keyMappings;
};


} // namespace resource


} // namespace reone
