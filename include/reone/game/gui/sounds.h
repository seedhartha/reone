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

class AudioFiles;
class TwoDa;
class TwoDas;

} // namespace resource

namespace audio {

class AudioBuffer;

}

namespace game {

class IGUISounds {
public:
    virtual ~IGUISounds() = default;

    virtual std::shared_ptr<audio::AudioBuffer> getOnClick() const = 0;
    virtual std::shared_ptr<audio::AudioBuffer> getOnEnter() const = 0;
};

class GUISounds : public IGUISounds, boost::noncopyable {
public:
    GUISounds(resource::AudioFiles &audioFiles, resource::TwoDas &twoDas) :
        _audioFiles(audioFiles),
        _twoDas(twoDas) {
    }

    ~GUISounds() { deinit(); }

    void init();
    void deinit();

    std::shared_ptr<audio::AudioBuffer> getOnClick() const override { return _onClick; }
    std::shared_ptr<audio::AudioBuffer> getOnEnter() const override { return _onEnter; }

private:
    resource::AudioFiles &_audioFiles;
    resource::TwoDas &_twoDas;

    std::shared_ptr<audio::AudioBuffer> _onClick;
    std::shared_ptr<audio::AudioBuffer> _onEnter;

    void loadSound(const resource::TwoDa &twoDa, const std::string &label, std::shared_ptr<audio::AudioBuffer> &sound);
};

} // namespace game

} // namespace reone
