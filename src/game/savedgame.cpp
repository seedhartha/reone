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

#include "savedgame.h"

#include <chrono>
#include <memory>

#include <boost/filesystem.hpp>

#include "../common/streamreader.h"
#include "../common/streamwriter.h"

#include "game.h"

namespace fs = boost::filesystem;

using namespace std;

namespace reone {

namespace game {

static constexpr char kSignature[] = "SAV";

SavedGame::SavedGame(const fs::path &path) : _path(path) {
}

void SavedGame::save(const Game *game, const string &name) {
    shared_ptr<ofstream> stream(new fs::ofstream(_path, ios::binary));
    StreamWriter writer(stream);

    writer.putCString(kSignature);
    writer.putInt64(chrono::system_clock::now().time_since_epoch().count());
    writer.putCString(name);
    writer.putCString(game->module()->name());
}

void SavedGame::peek() {
    shared_ptr<ifstream> stream(new fs::ifstream(_path, ios::binary));
    StreamReader reader(stream);

    string sign(reader.getCString());
    if (sign != kSignature) {
        throw runtime_error("Invalid SAV file signature");
    }

    _timestamp = reader.getInt64();
    _name = reader.getCString();
}

void SavedGame::load(Game *game) {
    shared_ptr<ifstream> stream(new fs::ifstream(_path, ios::binary));
    StreamReader reader(stream);

    string sign(reader.getCString());
    if (sign != kSignature) {
        throw runtime_error("Invalid SAV file signature");
    }

    reader.getInt64();
    reader.getCString();
    string moduleName(reader.getCString());

    game->setLoadFromSaveGame(true);
    game->scheduleModuleTransition(moduleName, "");
}

const string &SavedGame::name() const {
    return _name;
}

} // namespace game

} // namespace reone
