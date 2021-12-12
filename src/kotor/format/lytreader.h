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

#pragma once

#include "../../game/arealayout.h"

namespace reone {

namespace kotor {

class LytReader : boost::noncopyable {
public:
    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

    const game::AreaLayout &layout() const { return _layout; }

private:
    enum class State {
        None,
        Layout,
        Rooms
    };

    std::shared_ptr<std::istream> _in;
    boost::filesystem::path _path;
    State _state {State::None};
    int _roomCount {0};

    game::AreaLayout _layout;

    void load();
    void processLine(const std::string &line);

    void appendRoom(const std::vector<std::string> &tokens);
};

} // namespace kotor

} // namespace reone
