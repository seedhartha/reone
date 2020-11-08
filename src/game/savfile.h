/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <boost/filesystem/path.hpp>

namespace reone {

namespace game {

class Game;

class SavFile {
public:
    SavFile(const boost::filesystem::path &path);

    void save(const Game *game, const std::string &name);
    void peek();
    void load(Game *game);

    const std::string &name() const;

private:
    boost::filesystem::path _path;
    uint64_t _timestamp { 0 };
    std::string _name;

    SavFile(const SavFile &) = delete;
    SavFile &operator=(const SavFile &) = delete;
};

} // namespace game

} // namespace reone
