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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

#include "../audio/stream.h"
#include "../render/font.h"
#include "../render/model/model.h"
#include "../render/walkmesh.h"
#include "../script/program.h"

#include "2dafile.h"
#include "gfffile.h"
#include "keyfile.h"
#include "pefile.h"
#include "resourceprovider.h"
#include "tlkfile.h"

namespace reone {

namespace resource {

class Resources {
public:
    static Resources &instance();

    void init(GameVersion version, const boost::filesystem::path &gamePath);
    void deinit();

    void invalidateCache();
    void loadModule(const std::string &name);

    std::shared_ptr<ByteArray> get(const std::string &resRef, ResourceType type, bool logNotFound = true);
    std::shared_ptr<TwoDaTable> get2DA(const std::string &resRef);
    std::shared_ptr<GffStruct> getGFF(const std::string &resRef, ResourceType type);
    std::shared_ptr<ByteArray> getFromExe(uint32_t name, PEResourceType type);
    std::shared_ptr<TalkTable> getTalkTable(const std::string &resRef);

    std::string getString(int32_t ref) const;

    const std::vector<std::string> &moduleNames() const;

private:
    GameVersion _version { GameVersion::KotOR };
    boost::filesystem::path _gamePath;
    KeyFile _keyFile;
    TlkFile _tlkFile;
    PEFile _exeFile;
    std::vector<std::string> _moduleNames;
    std::vector<std::unique_ptr<IResourceProvider>> _providers;
    std::vector<std::unique_ptr<IResourceProvider>> _transientProviders;

    Resources() = default;
    Resources(const Resources &) = delete;
    ~Resources();

    Resources &operator=(const Resources &) = delete;

    void indexAudioFiles();
    void indexDirectory(const boost::filesystem::path &path);
    void indexErfFile(const boost::filesystem::path &path);
    void indexExeFile();
    void indexKeyFile();
    void indexOverrideDirectory();
    void indexTalkTable();
    void indexTexturePacks();
    void indexTransientErfFile(const boost::filesystem::path &path);
    void indexTransientRimFile(const boost::filesystem::path &path);
    void loadModuleNames();
    void stripDeveloperNotes(std::string &text) const;

    std::shared_ptr<ByteArray> get(const std::vector<std::unique_ptr<IResourceProvider>> &providers, const std::string &resRef, ResourceType type);
    inline std::string getCacheKey(const std::string &resRef, ResourceType type) const;
};

} // namespace resource

} // namespace reone
