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

#include "../pch.h"

namespace reone {

namespace resource {

class LytReader : boost::noncopyable {
public:
    struct Room {
        std::string name;
        glm::vec3 position;
    };

    struct DoorHook {
        std::string room;
        std::string name;
        glm::vec3 position;
    };

    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

    const std::vector<Room> &rooms() const { return _rooms; }
    const std::vector<DoorHook> &doorHooks() const { return _doorHooks; }

private:
    enum class State {
        None,
        Layout,
        Rooms,
        DoorHooks
    };

    std::shared_ptr<std::istream> _in;
    boost::filesystem::path _path;
    State _state { State::None };
    int _roomCount { 0 };
    int _doorHookCount { 0 };
    std::vector<Room> _rooms;
    std::vector<DoorHook> _doorHooks;

    void load();
    void processLine(const std::string &line);
    Room getRoom(const std::vector<std::string> &tokens) const;
    DoorHook getDoorHook(const std::vector<std::string> &tokens) const;
};

} // namespace resource

} // namespace reone
