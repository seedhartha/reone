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

#include "lytfile.h"

#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

void LytFile::load(const std::shared_ptr<std::istream> &in) {
    if (!in) {
        throw std::invalid_argument("LYT: invalid input stream");
    }
    _in = in;

    load();
}

void LytFile::load(const fs::path &path) {
    if (!fs::exists(path)) {
        throw std::runtime_error("LYT: file not found: " + path.string());
    }
    _in.reset(new fs::ifstream(path));
    _path = path;

    load();
}

void LytFile::load() {
    char buf[256];
    do {
        _in->getline(buf, sizeof(buf));
        processLine(std::string(buf));
    } while (!_in->eof());
}

void LytFile::processLine(const std::string &line) {
    std::string trimmed(line);
    boost::trim(trimmed);

    std::vector<std::string> tokens;
    boost::split(tokens, trimmed, boost::is_space(), boost::token_compress_on);

    const std::string &first = tokens[0];
    switch (_state) {
        case State::None:
            if (first == "beginlayout") {
                _state = State::Layout;
            }
            break;
        case State::Layout:
            if (first == "donelayout") {
                _state = State::None;
            } else if (first == "roomcount") {
                _roomCount = std::stoi(tokens[1]);
                if (_roomCount > 0) {
                    _rooms.reserve(_roomCount);
                    _state = State::Rooms;
                }
            } else if (first == "doorhookcount") {
                _doorHookCount = std::stoi(tokens[1]);
                if (_doorHookCount > 0) {
                    _doorHooks.reserve(_doorHookCount);
                    _state = State::DoorHooks;
                }
            }
            break;
        case State::Rooms:
            _rooms.push_back(getRoom(tokens));
            if (_rooms.size() == _roomCount) {
                _state = State::Layout;
            }
            break;
        case State::DoorHooks:
            _doorHooks.push_back(getDoorHook(tokens));
            if (_doorHooks.size() == _doorHookCount) {
                _state = State::Layout;
            }
            break;
    }
}

LytFile::Room LytFile::getRoom(const std::vector<std::string> &tokens) const {
    Room room;
    room.name = boost::to_lower_copy(tokens[0]);
    room.position = glm::vec3(
        std::stof(tokens[1]),
        std::stof(tokens[2]),
        std::stof(tokens[3]));

    return std::move(room);
}

LytFile::DoorHook LytFile::getDoorHook(const std::vector<std::string> &tokens) const {
    DoorHook door;
    door.room = boost::to_lower_copy(tokens[0]);
    door.name = boost::to_lower_copy(tokens[1]);
    door.position = glm::vec3(
        std::stof(tokens[2]),
        std::stof(tokens[3]),
        std::stof(tokens[4]));

    return std::move(door);
}

const std::vector<LytFile::Room> &LytFile::rooms() const {
    return _rooms;
}

const std::vector<LytFile::DoorHook> &LytFile::doorHooks() const {
    return _doorHooks;
}

} // namespace resources

} // namespace reone
