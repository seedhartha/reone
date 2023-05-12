/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/format/visreader.h"

#include "reone/system/stream/input.h"

using namespace std;

namespace reone {

namespace game {

void VisReader::load(IInputStream &in) {
    char buf[32];
    do {
        in.readLine(buf, sizeof(buf));

        string line(buf);
        boost::trim(line);

        if (line.empty())
            continue;

        processLine(line);
    } while (!in.eof());
}

void VisReader::processLine(const string &line) {
    vector<string> tokens;
    boost::split(tokens, line, boost::is_space(), boost::token_compress_on);

    if (tokens.size() == 2) {
        _roomFrom = boost::to_lower_copy(tokens[0]);
    } else {
        string room(boost::to_lower_copy(tokens[0]));
        _visibility.insert(make_pair(_roomFrom, move(room)));
    }
}

} // namespace game

} // namespace reone
