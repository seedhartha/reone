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

#include "reone/audio/clip.h"

namespace reone {

namespace resource {

class AudioClips;
class TwoDas;

} // namespace resource

namespace game {

struct FootstepTypeSounds {
    std::vector<std::shared_ptr<audio::AudioClip>> dirt;
    std::vector<std::shared_ptr<audio::AudioClip>> grass;
    std::vector<std::shared_ptr<audio::AudioClip>> stone;
    std::vector<std::shared_ptr<audio::AudioClip>> wood;
    std::vector<std::shared_ptr<audio::AudioClip>> water;
    std::vector<std::shared_ptr<audio::AudioClip>> carpet;
    std::vector<std::shared_ptr<audio::AudioClip>> metal;
    std::vector<std::shared_ptr<audio::AudioClip>> leaves;
};

class IFootstepSounds {
public:
    virtual ~IFootstepSounds() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<FootstepTypeSounds> get(uint32_t key) = 0;
};

class FootstepSounds : public IFootstepSounds {
public:
    FootstepSounds(
        resource::AudioClips &audioClips,
        resource::TwoDas &twoDas) :
        _audioClips(audioClips),
        _twoDas(twoDas) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<FootstepTypeSounds> get(uint32_t key) override {
        auto maybeObject = _objects.find(key);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(key);
        return _objects.insert(make_pair(key, std::move(object))).first->second;
    }

private:
    resource::AudioClips &_audioClips;
    resource::TwoDas &_twoDas;

    std::unordered_map<uint32_t, std::shared_ptr<FootstepTypeSounds>> _objects;

    std::shared_ptr<FootstepTypeSounds> doGet(uint32_t type);
};

} // namespace game

} // namespace reone
