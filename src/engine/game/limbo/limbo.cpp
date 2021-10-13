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

#include "limbo.h"

#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../resource/resources.h"

#include "../core/script/runner.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kDataDirectoryName[] = "data";
static constexpr char kModulesDirectoryName[] = "modules";
static constexpr char kStartScriptResRef[] = "start";

void Limbo::initResourceProviders() {
    fs::path dataDir(getPathIgnoreCase(_path, kDataDirectoryName));
    if (!dataDir.empty()) {
        _resources.indexDirectory(dataDir);
    }
}

void Limbo::loadModuleNames() {
    fs::path modules(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modules.empty()) {
        throw ValidationException("Modules directory not found");
    }
    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(boost::to_lower_copy(entry.path().filename().string()));
        if (boost::ends_with(filename, ".mod")) {
            string moduleName(boost::to_lower_copy(filename.substr(0, filename.size() - 4)));
            _moduleNames.insert(move(moduleName));
        }
    }
}

void Limbo::start() {
    _scriptRunner.run(kStartScriptResRef);
}

void Limbo::loadModuleResources(const string &moduleName) {
    _resources.invalidateCache();
    _resources.clearTransientProviders();

    fs::path modulesPath(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modulesPath.empty()) {
        throw ValidationException("Modules directory not found");
    }

    fs::path modPath(getPathIgnoreCase(modulesPath, moduleName + ".mod"));
    if (modPath.empty()) {
        throw ValidationException("Module MOD archive not found");
    }
    _resources.indexErfFile(modPath);
}

void Limbo::openInGame() {
    changeScreen(GameScreen::InGame);
}

void Limbo::changeScreen(GameScreen screen) {
    _screen = screen;
}

} // namespace game

} // namespace reone
