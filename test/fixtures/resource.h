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

#pragma once

#include <gmock/gmock.h>

#include "reone/resource/di/services.h"
#include "reone/resource/director.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/audioclips.h"
#include "reone/resource/provider/cursors.h"
#include "reone/resource/provider/dialogs.h"
#include "reone/resource/provider/fonts.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/layouts.h"
#include "reone/resource/provider/lips.h"
#include "reone/resource/provider/ltrs.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/movies.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/scripts.h"
#include "reone/resource/provider/shaders.h"
#include "reone/resource/provider/soundsets.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/provider/visibilities.h"
#include "reone/resource/provider/walkmeshes.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"

namespace reone {

namespace resource {

class MockGffs : public IGffs, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Gff>, get, (const std::string &resRef, ResType type), (override));
};

class MockResources : public IResources, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(void, clearLocal, (), (override));
    MOCK_METHOD(void, addKEY, (const std::filesystem::path &path), (override));
    MOCK_METHOD(void, addERF, (const std::filesystem::path &path, bool local), (override));
    MOCK_METHOD(void, addRIM, (const std::filesystem::path &path, bool local), (override));
    MOCK_METHOD(void, addEXE, (const std::filesystem::path &path), (override));
    MOCK_METHOD(void, addFolder, (const std::filesystem::path &path), (override));

    MOCK_METHOD(Resource, get, (const ResourceId &id), (override));
    MOCK_METHOD(std::optional<Resource>, find, (const ResourceId &id), (override));
};

class MockStrings : public IStrings, boost::noncopyable {
public:
    MOCK_METHOD(std::string, getText, (int strRef), (override));
    MOCK_METHOD(std::string, getSound, (int strRef), (override));
};

class MockTwoDAs : public ITwoDAs, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<TwoDA>, get, (const std::string &key), (override));
};

class MockScripts : public IScripts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<script::ScriptProgram>, get, (const std::string &key), (override));
};

class MockMovies : public IMovies, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<movie::IMovie>, get, (const std::string &name), (override));
};

class MockAudioClips : public IAudioClips, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<audio::AudioClip>, get, (const std::string &key), (override));
};

class MockCursors : public ICursors, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<graphics::Cursor>, get, (resource::CursorType type), (override));
};

class MockFonts : public IFonts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<graphics::Font>, get, (const std::string &key), (override));
};

class MockLips : public ILips, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<graphics::LipAnimation>, get, (const std::string &key), (override));
};

class MockModels : public IModels, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<graphics::Model>, get, (const std::string &resRef), (override));
};

class MockTextures : public ITextures, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));

    MOCK_METHOD(std::shared_ptr<graphics::Texture>, get, (const std::string &resRef, graphics::TextureUsage usage), (override));
};

class MockWalkmeshes : public IWalkmeshes, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<graphics::Walkmesh>, get, (const std::string &resRef, ResType type), (override));
};

class MockDialogs : public IDialogs, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Dialog>, get, (const std::string &key), (override));
};

class MockLayouts : public ILayouts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Layout>, get, (const std::string &key), (override));
};

class MockPaths : public IPaths, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Path>, get, (const std::string &key), (override));
};

class MockSoundSets : public ISoundSets, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<SoundSet>, get, (const std::string &key), (override));
};

class MockVisiblities : public IVisibilities, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Visibility>, get, (const std::string &key), (override));
};

class MockLtrs : public ILtrs, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Ltr>, get, (const ResRef &resRef), (override));
};

class MockShaders : public IShaders, boost::noncopyable {
};

class MockResourceDirector : public IResourceDirector, boost::noncopyable {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(void, onModuleLoad, (const std::string &name), (override));
    MOCK_METHOD(std::set<std::string>, moduleNames, (), (override));
};

class TestResourceModule : boost::noncopyable {
public:
    void init() {
        _gffs = std::make_unique<MockGffs>();
        _resources = std::make_unique<MockResources>();
        _strings = std::make_unique<MockStrings>();
        _twoDas = std::make_unique<MockTwoDAs>();
        _scripts = std::make_unique<MockScripts>();
        _movies = std::make_unique<MockMovies>();
        _audioClips = std::make_unique<MockAudioClips>();
        _cursors = std::make_unique<MockCursors>();
        _fonts = std::make_unique<MockFonts>();
        _lips = std::make_unique<MockLips>();
        _models = std::make_unique<MockModels>();
        _textures = std::make_unique<MockTextures>();
        _walkmeshes = std::make_unique<MockWalkmeshes>();
        _dialogs = std::make_unique<MockDialogs>();
        _layouts = std::make_unique<MockLayouts>();
        _paths = std::make_unique<MockPaths>();
        _soundSets = std::make_unique<MockSoundSets>();
        _visibilities = std::make_unique<MockVisiblities>();
        _ltrs = std::make_unique<MockLtrs>();
        _shaders = std::make_unique<MockShaders>();
        _director = std::make_unique<MockResourceDirector>();

        _services = std::make_unique<ResourceServices>(
            *_gffs,
            *_resources,
            *_strings,
            *_twoDas,
            *_scripts,
            *_movies,
            *_audioClips,
            *_cursors,
            *_fonts,
            *_lips,
            *_models,
            *_textures,
            *_walkmeshes,
            *_dialogs,
            *_layouts,
            *_paths,
            *_soundSets,
            *_visibilities,
            *_ltrs,
            *_shaders,
            *_director);
    }

    MockGffs &gffs() {
        return *_gffs;
    }

    MockTwoDAs &twoDas() {
        return *_twoDas;
    }

    MockStrings &strings() {
        return *_strings;
    }

    MockScripts &scripts() {
        return *_scripts;
    }

    MockMovies &movies() {
        return *_movies;
    }

    MockAudioClips &audioClips() {
        return *_audioClips;
    }

    MockResourceDirector &director() {
        return *_director;
    }

    ResourceServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockGffs> _gffs;
    std::unique_ptr<MockResources> _resources;
    std::unique_ptr<MockStrings> _strings;
    std::unique_ptr<MockTwoDAs> _twoDas;
    std::unique_ptr<MockScripts> _scripts;
    std::unique_ptr<MockMovies> _movies;
    std::unique_ptr<MockAudioClips> _audioClips;
    std::unique_ptr<MockCursors> _cursors;
    std::unique_ptr<MockFonts> _fonts;
    std::unique_ptr<MockLips> _lips;
    std::unique_ptr<MockModels> _models;
    std::unique_ptr<MockTextures> _textures;
    std::unique_ptr<MockWalkmeshes> _walkmeshes;
    std::unique_ptr<MockDialogs> _dialogs;
    std::unique_ptr<MockLayouts> _layouts;
    std::unique_ptr<MockPaths> _paths;
    std::unique_ptr<MockSoundSets> _soundSets;
    std::unique_ptr<MockVisiblities> _visibilities;
    std::unique_ptr<MockLtrs> _ltrs;
    std::unique_ptr<MockShaders> _shaders;
    std::unique_ptr<MockResourceDirector> _director;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
