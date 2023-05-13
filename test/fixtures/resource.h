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

#include "reone/resource/di/services.h"

namespace reone {

namespace resource {

class MockGffs : public IGffs, boost::noncopyable {
public:
    void invalidate() {
    }

    std::shared_ptr<Gff> get(const std::string &resRef, ResourceType type) {
        return nullptr;
    }
};

class MockResources : public IResources, boost::noncopyable {
public:
    void clearTransientProviders() override {}

    void indexKeyFile(const boost::filesystem::path &path) override {}
    void indexErfFile(const boost::filesystem::path &path, bool transient = false) override {}
    void indexRimFile(const boost::filesystem::path &path, bool transient = false) override {}
    void indexDirectory(const boost::filesystem::path &path) override {}
    void indexExeFile(const boost::filesystem::path &path) override {}

    std::shared_ptr<ByteArray> get(const std::string &resRef, ResourceType type, bool logNotFound = true) override {
        return nullptr;
    }
};

class MockStrings : public IStrings, boost::noncopyable {
public:
    std::string get(int strRef) override {
        return "";
    }

    std::string getSound(int strRef) override {
        return "";
    }
};

class MockTwoDas : public ITwoDas, boost::noncopyable {
public:
    void invalidate() override {}

    std::shared_ptr<TwoDa> get(const std::string &key) override {
        return nullptr;
    }
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
