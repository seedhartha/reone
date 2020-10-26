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

#include <boost/filesystem.hpp>

#include "glm/glm.hpp"

namespace reone {

namespace resource {

class LytFile {
public:
    struct Room {
        std::string name;
        glm::vec3 position { 0.0f };
    };

    struct DoorHook {
        std::string room;
        std::string name;
        glm::vec3 position { 0.0f };
    };

    LytFile() = default;

    void load(const std::shared_ptr<std::istream> &in);
    void load(const boost::filesystem::path &path);

    const std::vector<Room> &rooms() const;
    const std::vector<DoorHook> &doorHooks() const;

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

    LytFile(const LytFile &) = delete;
    LytFile &operator=(const LytFile &) = delete;

    void load();
    void processLine(const std::string &line);
    Room getRoom(const std::vector<std::string> &tokens) const;
    DoorHook getDoorHook(const std::vector<std::string> &tokens) const;
};

} // namespace resource

} // namespace reone
