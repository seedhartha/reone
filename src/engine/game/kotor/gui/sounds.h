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

#include "../../../audio/files.h"
#include "../../../audio/stream.h"
#include "../../../resource/2da.h"
#include "../../../resource/resources.h"

namespace reone {

namespace game {

class GUISounds : boost::noncopyable {
public:
    GUISounds(audio::AudioFiles &audioFiles, resource::Resources &resources);
    ~GUISounds();

    void init();
    void deinit();

    std::shared_ptr<audio::AudioStream> getOnClick() const { return _onClick; }
    std::shared_ptr<audio::AudioStream> getOnEnter() const { return _onEnter; }

private:
    audio::AudioFiles &_audioFiles;
    resource::Resources &_resources;

    std::shared_ptr<audio::AudioStream> _onClick;
    std::shared_ptr<audio::AudioStream> _onEnter;

    void loadSound(const resource::TwoDA &twoDa, const std::string &label, std::shared_ptr<audio::AudioStream> &sound);
};

} // namespace game

} // namespace reone
