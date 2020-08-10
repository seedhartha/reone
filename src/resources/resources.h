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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

#include "../audio/stream.h"
#include "../render/font.h"
#include "../render/model.h"
#include "../render/walkmesh.h"
#include "../script/program.h"

#include "2dafile.h"
#include "gfffile.h"
#include "keyfile.h"
#include "tlkfile.h"

namespace reone {

namespace resources {

class ResourceManager {
public:
    static ResourceManager &instance();

    void init(GameVersion version, const boost::filesystem::path &gamePath);
    void deinit();
    void clearCaches();
    void loadModule(const std::string &name);

    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type);
    std::shared_ptr<TwoDaTable> find2DA(const std::string &resRef);
    std::shared_ptr<GffStruct> findGFF(const std::string &resRef, ResourceType type);
    std::shared_ptr<TalkTable> findTalkTable(const std::string &resRef);
    std::shared_ptr<audio::AudioStream> findAudio(const std::string &resRef);
    std::shared_ptr<render::Model> findModel(const std::string &resRef);
    std::shared_ptr<render::Walkmesh> findWalkmesh(const std::string &resRef, ResourceType type);
    std::shared_ptr<render::Texture> findTexture(const std::string &resRef, render::TextureType type);
    std::shared_ptr<render::Font> findFont(const std::string &resRef);
    std::shared_ptr<script::ScriptProgram> findScript(const std::string &resRef);

    const TalkTableString &getString(int32_t ref) const;

    const std::vector<std::string> &moduleNames() const;

private:
    GameVersion _version { GameVersion::KotOR };
    boost::filesystem::path _gamePath;
    KeyFile _keyFile;
    TlkFile _tlkFile;
    std::vector<std::string> _moduleNames;
    std::vector<std::unique_ptr<IResourceProvider>> _providers;
    std::vector<std::unique_ptr<IResourceProvider>> _transientProviders;

    ResourceManager() = default;
    ResourceManager(const ResourceManager &) = delete;
    ~ResourceManager();

    ResourceManager &operator=(const ResourceManager &) = delete;

    void addErfProvider(const boost::filesystem::path &path);
    void addTransientRimProvider(const boost::filesystem::path &path);
    void addTransientErfProvider(const boost::filesystem::path &path);
    void addFolderProvider(const boost::filesystem::path &path);
    void initModuleNames();
    inline std::string getCacheKey(const std::string &resRef, ResourceType type) const;
    std::shared_ptr<ByteArray> find(const std::vector<std::unique_ptr<IResourceProvider>> &providers, const std::string &resRef, ResourceType type);
};

#define ResMan ResourceManager::instance()

} // namespace resources

} // namespace reone
