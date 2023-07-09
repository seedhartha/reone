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

namespace reone {

namespace resource {

class MockGffs : public IGffs, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Gff>, get, (const std::string &resRef, ResourceType type), (override));
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

    MOCK_METHOD(ByteBuffer, get, (const ResourceId &id), (override));
    MOCK_METHOD(std::optional<ByteBuffer>, find, (const ResourceId &id), (override));
};

class MockStrings : public IStrings, boost::noncopyable {
public:
    MOCK_METHOD(std::string, get, (int strRef), (override));
    MOCK_METHOD(std::string, getSound, (int strRef), (override));
};

class MockTwoDas : public ITwoDas, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<TwoDa>, get, (const std::string &key), (override));
};

class TestResourceModule : boost::noncopyable {
public:
    void init() {
        _gffs = std::make_unique<MockGffs>();
        _resources = std::make_unique<MockResources>();
        _strings = std::make_unique<MockStrings>();
        _twoDas = std::make_unique<MockTwoDas>();

        _services = std::make_unique<ResourceServices>(
            *_gffs,
            *_resources,
            *_strings,
            *_twoDas);
    }

    MockGffs &gffs() {
        return *_gffs;
    }

    MockStrings &strings() {
        return *_strings;
    }

    ResourceServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockGffs> _gffs;
    std::unique_ptr<MockResources> _resources;
    std::unique_ptr<MockStrings> _strings;
    std::unique_ptr<MockTwoDas> _twoDas;

    std::unique_ptr<ResourceServices> _services;
};

} // namespace resource

} // namespace reone
