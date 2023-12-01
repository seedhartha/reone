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

#include "reone/resource/2das.h"
#include "reone/resource/audio/files.h"
#include "reone/resource/audio/player.h"
#include "reone/resource/di/services.h"
#include "reone/resource/format/mp3reader.h"
#include "reone/resource/gffs.h"
#include "reone/resource/movies.h"
#include "reone/resource/resources.h"
#include "reone/resource/scripts.h"
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

class MockTwoDas : public ITwoDas, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<TwoDa>, get, (const std::string &key), (override));
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

class MockAudioFiles : public IAudioFiles, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<audio::AudioBuffer>, get, (const std::string &key), (override));
};

class MockAudioPlayer : public IAudioPlayer, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<audio::AudioSource>, play, (const std::string &resRef, audio::AudioType type, bool loop, float gain, bool positional, glm::vec3 position), (override));
    MOCK_METHOD(std::shared_ptr<audio::AudioSource>, play, (std::shared_ptr<audio::AudioBuffer> stream, audio::AudioType type, bool loop, float gain, bool positional, glm::vec3 position), (override));
};

class MockMp3Reader : public Mp3Reader {
public:
    MOCK_METHOD(void, load, (IInputStream & stream), (override));
};

class MockMp3ReaderFactory : public IMp3ReaderFactory, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Mp3Reader>, create, (), (override));
};

class TestResourceModule : boost::noncopyable {
public:
    void init() {
        _gffs = std::make_unique<MockGffs>();
        _resources = std::make_unique<MockResources>();
        _strings = std::make_unique<MockStrings>();
        _twoDas = std::make_unique<MockTwoDas>();
        _scripts = std::make_unique<MockScripts>();
        _movies = std::make_unique<MockMovies>();
        _files = std::make_unique<MockAudioFiles>();
        _player = std::make_unique<MockAudioPlayer>();

        _services = std::make_unique<ResourceServices>(
            *_gffs,
            *_resources,
            *_strings,
            *_twoDas,
            *_scripts,
            *_movies,
            *_files,
            *_player);
    }

    MockGffs &gffs() {
        return *_gffs;
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

    MockAudioFiles &audioFiles() {
        return *_files;
    }

    MockAudioPlayer &audioPlayer() {
        return *_player;
    }

    ResourceServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockGffs> _gffs;
    std::unique_ptr<MockResources> _resources;
    std::unique_ptr<MockStrings> _strings;
    std::unique_ptr<MockTwoDas> _twoDas;
    std::unique_ptr<MockScripts> _scripts;
    std::unique_ptr<MockMovies> _movies;
    std::unique_ptr<MockAudioFiles> _files;
    std::unique_ptr<MockAudioPlayer> _player;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
