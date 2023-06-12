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

#include "mainviewmodel.h"

#include "reone/resource/format/keyreader.h"
#include "reone/system/pathutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/tools/2da.h"
#include "reone/tools/audio.h"
#include "reone/tools/erf.h"
#include "reone/tools/gff.h"
#include "reone/tools/keybif.h"
#include "reone/tools/lip.h"
#include "reone/tools/ncs.h"
#include "reone/tools/rim.h"
#include "reone/tools/ssf.h"
#include "reone/tools/tlk.h"
#include "reone/tools/tpc.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;

namespace reone {

static const set<string> kFilesSubdirectoryWhitelist {
    "data", "lips", "localvault", "modules", "movies", "override", "rims", "saves", "texturepacks", //
    "streammusic", "streamsounds", "streamwaves", "streamvoice"};

static const set<string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const set<string> kFilesExtensionBlacklist {
    ".key",                                         //
    ".lnk", ".bat", ".exe", ".dll", ".ini", ".ico", //
    ".zip", ".pdf",                                 //
    ".hashdb", ".info", ".script", ".dat", ".msg", ".sdb", ".ds_store"};

void MainViewModel::loadGameDirectory() {
    auto tslExePath = getPathIgnoreCase(_gamePath, "swkotor2.exe", false);
    _gameId = !tslExePath.empty() ? GameID::TSL : GameID::KotOR;

    auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key", false);
    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    for (auto &file : boost::filesystem::directory_iterator(_gamePath)) {
        auto filename = boost::to_lower_copy(file.path().filename().string());
        auto extension = boost::to_lower_copy(file.path().extension().string());
        bool container;
        if ((file.status().type() == boost::filesystem::directory_file && kFilesSubdirectoryWhitelist.count(filename) > 0) ||
            (file.status().type() == boost::filesystem::regular_file && kFilesArchiveExtensions.count(extension) > 0)) {
            container = true;
        } else if (file.status().type() == boost::filesystem::regular_file && (kFilesExtensionBlacklist.count(extension) == 0 && extension != ".txt")) {
            container = false;
        } else {
            continue;
        }
        auto item = GameDirectoryItem();
        item.displayName = filename;
        item.path = file.path();
        item.container = container;
        if (extension == ".tlk") {
            auto resRef = filename.substr(0, filename.size() - 4);
            item.resId = make_unique<ResourceId>(resRef, ResourceType::Tlk);
        }
        _gameDirItems.push_back(item);
    }
}

void MainViewModel::loadTools() {
    _tools.clear();
    _tools.push_back(make_shared<KeyBifTool>());
    _tools.push_back(make_shared<ErfTool>());
    _tools.push_back(make_shared<RimTool>());
    _tools.push_back(make_shared<TwoDaTool>());
    _tools.push_back(make_shared<TlkTool>());
    _tools.push_back(make_shared<LipTool>());
    _tools.push_back(make_shared<SsfTool>());
    _tools.push_back(make_shared<GffTool>());
    _tools.push_back(make_shared<TpcTool>());
    _tools.push_back(make_shared<AudioTool>());
    _tools.push_back(make_shared<NcsTool>(_gameId));
}

bool MainViewModel::invokeTool(Operation operation,
                               const boost::filesystem::path &srcPath,
                               const boost::filesystem::path &destPath) {
    for (auto &tool : _tools) {
        if (!tool->supports(operation, srcPath)) {
            continue;
        }
        tool->invoke(operation, srcPath, destPath, _gamePath);
        return true;
    }
    return false;
}

void MainViewModel::onViewCreated() {
    loadTools();
}

void MainViewModel::onViewDestroyed() {
}

void MainViewModel::onGameDirectoryChanged(boost::filesystem::path path) {
    _gamePath = path;

    loadGameDirectory();
    loadTools();
}

void MainViewModel::onGameDirectoryItemIdentified(int index, GameDirectoryItemId id) {
    _gameDirItems[index].id = id;
    _idToGameDirItem[id] = &_gameDirItems[index];
}

} // namespace reone
