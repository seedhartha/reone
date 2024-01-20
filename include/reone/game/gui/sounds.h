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

namespace reone {

namespace resource {

class AudioClips;
class TwoDA;
class TwoDAs;

} // namespace resource

namespace audio {

class AudioClip;

}

namespace game {

class IGUISounds {
public:
    virtual ~IGUISounds() = default;

    virtual std::shared_ptr<audio::AudioClip> getOnClick() const = 0;
    virtual std::shared_ptr<audio::AudioClip> getOnEnter() const = 0;
};

class GUISounds : public IGUISounds, boost::noncopyable {
public:
    GUISounds(resource::AudioClips &audioClips, resource::TwoDAs &twoDas) :
        _audioClips(audioClips),
        _twoDas(twoDas) {
    }

    ~GUISounds() { deinit(); }

    void init();
    void deinit();

    std::shared_ptr<audio::AudioClip> getOnClick() const override { return _onClick; }
    std::shared_ptr<audio::AudioClip> getOnEnter() const override { return _onEnter; }

private:
    resource::AudioClips &_audioClips;
    resource::TwoDAs &_twoDas;

    std::shared_ptr<audio::AudioClip> _onClick;
    std::shared_ptr<audio::AudioClip> _onEnter;

    void loadSound(const resource::TwoDA &twoDa, const std::string &label, std::shared_ptr<audio::AudioClip> &sound);
};

} // namespace game

} // namespace reone
