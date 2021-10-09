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

#include "sounds.h"

#include "../../../resource/format/2dareader.h"

using namespace std;

using namespace reone::audio;
using namespace reone::resource;

namespace reone {

namespace game {

GUISounds::GUISounds(AudioFiles &audioFiles, Resources &resources) :
    _audioFiles(audioFiles), _resources(resources) {
}

void GUISounds::init() {
    shared_ptr<TwoDA> sounds(_resources.get2DA("guisounds"));
    loadSound(*sounds, "Clicked_Default", _onClick);
    loadSound(*sounds, "Entered_Default", _onEnter);
}

void GUISounds::loadSound(const TwoDA &twoDa, const string &label, shared_ptr<AudioStream> &sound) {
    int row = twoDa.indexByCellValue("label", label);
    if (row != -1) {
        sound = _audioFiles.get(twoDa.getString(row, "soundresref"));
    }
}

GUISounds::~GUISounds() {
    deinit();
}

void GUISounds::deinit() {
    _onClick.reset();
    _onEnter.reset();
}

} // namespace game

} // namespace reone
