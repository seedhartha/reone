/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "lytreader.h"

#include "../../common/stream/fileinput.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

void LytReader::load(const shared_ptr<IInputStream> &in) {
    if (!in) {
        throw invalid_argument("LYT: invalid input stream");
    }
    _in = in;

    load();
}

void LytReader::load(const fs::path &path) {
    if (!fs::exists(path)) {
        throw runtime_error("LYT: file not found: " + path.string());
    }
    _in = make_shared<FileInputStream>(path);
    _path = path;

    load();
}

void LytReader::load() {
    char buf[256];
    do {
        char *pch = buf;
        int ch;
        do {
            ch = _in->readByte();
            *(pch++) = ch;
        } while (ch != -1 && ch != '\n');
        processLine(string(buf, pch - buf - 1));
    } while (!_in->eof());
}

void LytReader::processLine(const string &line) {
    vector<string> tokens;
    string trimLine(boost::trim_copy(line));
    boost::split(tokens, trimLine, boost::is_space(), boost::token_compress_on);

    const string &first = tokens[0];
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
            _roomCount = stoi(tokens[1]);
            if (_roomCount > 0) {
                _layout.rooms.reserve(_roomCount);
                _state = State::Rooms;
            }
        }
        break;
    case State::Rooms:
        appendRoom(tokens);
        if (_layout.rooms.size() == _roomCount) {
            _state = State::Layout;
        }
        break;
    }
}

void LytReader::appendRoom(const vector<string> &tokens) {
    Layout::Room room;
    room.name = boost::to_lower_copy(tokens[0]);
    room.position = glm::vec3(
        stof(tokens[1]),
        stof(tokens[2]),
        stof(tokens[3]));
    _layout.rooms.push_back(move(room));
}

} // namespace game

} // namespace reone
