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

#include "../types.h"

namespace reone {

namespace resource {

class Resources;
class Strings;
class AudioClips;

class ISoundSets {
public:
    virtual ~ISoundSets() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<SoundSet> get(const std::string &key) = 0;
};

class SoundSets : public ISoundSets {
public:
    SoundSets(
        AudioClips &audioClips,
        Resources &resources,
        Strings &strings) :
        _audioClips(audioClips),
        _resources(resources),
        _strings(strings) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<SoundSet> get(const std::string &key) override {
        auto maybeObject = _objects.find(key);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(key);
        return _objects.insert(make_pair(key, std::move(object))).first->second;
    }

private:
    AudioClips &_audioClips;
    Resources &_resources;
    Strings &_strings;

    std::unordered_map<std::string, std::shared_ptr<SoundSet>> _objects;

    std::shared_ptr<SoundSet> doGet(std::string resRef);
};

} // namespace resource

} // namespace reone
